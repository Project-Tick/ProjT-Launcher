// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2026 Project Tick
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include "ComponentUpdateTask.h"
#include <algorithm>

#include "Component.h"
#include "ComponentUpdateTask_p.h"
#include "PackProfile.h"
#include "PackProfile_p.h"
#include "ProblemProvider.h"
#include "Version.h"
#include "cassert"
#include "meta/Index.hpp"
#include "meta/Version.hpp"
#include "minecraft/MinecraftInstance.h"
#include "minecraft/OneSixVersionFormat.h"
#include "minecraft/ProfileUtils.h"
#include "net/Mode.h"

#include "Application.h"
#include "tasks/Task.h"

#include "minecraft/Logging.h"
#include "meta/VersionList.hpp"
#include "tasks/SequentialTask.h"

/*
 * This is responsible for loading the components of a component list AND resolving dependency issues between them
 */

/*
 * NOTE: The 'one shot async task' nature of this implementation is a known limitation.
 * Ideally, this should be refactored into a reactor/state machine that receives input
 * from the application and dynamically adapts to changing requirements.
 *
 * The reactor should be the only entry point for manipulating the PackProfile.
 * See: https://en.wikipedia.org/wiki/Reactor_pattern
 *
 * Current implementation logic:
 * - Operates on a snapshot of the PackProfile state.
 * - Merges results as long as the snapshot and PackProfile haven't diverged during execution.
 * - Requires a restart if the component list changes mid-operation.
 */

/*
 * Or make this operate on a snapshot of the PackProfile state, then merge results in as long as the snapshot and
 * PackProfile didn't change? If the component list changes, start over.
 */

ComponentUpdateTask::ComponentUpdateTask(Mode mode, Net::Mode netmode, PackProfile* list) : Task()
{
	d.reset(new ComponentUpdateTaskData);
	d->m_profile = list;
	d->mode		 = mode;
	d->netmode	 = netmode;
}

ComponentUpdateTask::~ComponentUpdateTask()
{}

void ComponentUpdateTask::executeTask()
{
	qCDebug(instanceProfileResolveC) << "Loading components";
	loadComponents();
}

namespace
{
	enum class LoadResult
	{
		LoadedLocal,
		RequiresRemote,
		Failed
	};

	LoadResult composeLoadResult(LoadResult a, LoadResult b)
	{
		if (a < b)
		{
			return b;
		}
		return a;
	}

	static LoadResult loadComponent(ComponentPtr component, Task::Ptr& loadTask, Net::Mode netmode)
	{
		if (component->m_loaded)
		{
			qCDebug(instanceProfileResolveC) << component->getName() << "is already loaded";
			return LoadResult::LoadedLocal;
		}

		LoadResult result		 = LoadResult::Failed;
		auto customPatchFilename = component->getFilename();
		if (QFile::exists(customPatchFilename))
		{
			// if local file exists...

			// check for uid problems inside...
			bool fileChanged = false;
			auto file		 = ProfileUtils::parseJsonFile(QFileInfo(customPatchFilename), false);
			if (file->uid != component->m_uid)
			{
				file->uid	= component->m_uid;
				fileChanged = true;
			}
			if (fileChanged)
			{
				// Ensure we don't ignore failures when writing back a fixed json file.
				bool saved =
					ProfileUtils::saveJsonFile(OneSixVersionFormat::versionFileToJson(file), customPatchFilename);
				if (!saved)
				{
					qCWarning(instanceProfileResolveC)
						<< "Failed to save modified component file:" << customPatchFilename;
				}
			}

			component->m_file	= file;
			component->m_loaded = true;
			result				= LoadResult::LoadedLocal;
		}
		else
		{
			auto metaVersion		 = APPLICATION->metadataIndex()->version(component->m_uid, component->m_version);
			component->m_metaVersion = metaVersion;
			if (metaVersion->isFullyLoaded())
			{
				component->m_loaded = true;
				result				= LoadResult::LoadedLocal;
			}
			else
			{
				loadTask =
					APPLICATION->metadataIndex()->loadVersionTask(component->m_uid, component->m_version, netmode);
				loadTask->start();
				if (netmode == Net::Mode::Online)
					result = LoadResult::RequiresRemote;
				else if (metaVersion->isFullyLoaded())
					result = LoadResult::LoadedLocal;
				else
					result = LoadResult::Failed;
			}
		}
		return result;
	}

	static LoadResult loadPackProfile(ComponentPtr component, Task::Ptr& loadTask, Net::Mode netmode)
	{
		auto index = APPLICATION->metadataIndex();

		// If index is not yet synchronized and we're online, we need to load it first
		if (index->state() != projt::meta::MetaEntity::State::Synchronized)
		{
			if (netmode == Net::Mode::Offline)
			{
				qCWarning(instanceProfileResolveC) << "Metadata index not available offline for" << component->m_uid;
				return LoadResult::Failed;
			}

			// Create a sequential task that first loads the index, then the version list
			auto seq = makeShared<SequentialTask>(
				ComponentUpdateTask::tr("Loading metadata for %1").arg(component->getName()));
			seq->addTask(index->createLoadTask(netmode));

			// After index loads, we need to load the version list
			// Use a callback-based approach by connecting after index load completes
			auto indexLoadTask = index->createLoadTask(netmode);

			// Create a task that will load version list after index is ready
			class DeferredVersionListLoader : public Task
			{
			  public:
				DeferredVersionListLoader(ComponentPtr comp, Net::Mode mode) : m_component(comp), m_mode(mode)
				{}

				void executeTask() override
				{
					auto versionList = m_component->getVersionList();
					if (!versionList)
					{
						emitFailed(tr("Component %1 not found in metadata index").arg(m_component->m_uid));
						return;
					}
					if (versionList->isLoaded())
					{
						emitSucceeded();
						return;
					}
					m_innerTask = versionList->createLoadTask(m_mode);
					connect(m_innerTask.get(), &Task::succeeded, this, [this]() { emitSucceeded(); });
					connect(m_innerTask.get(),
							&Task::failed,
							this,
							[this](const QString& reason) { emitFailed(reason); });
					connect(m_innerTask.get(), &Task::progress, this, &Task::setProgress);
					connect(m_innerTask.get(), &Task::status, this, &Task::setStatus);
					m_innerTask->start();
				}

				bool canAbort() const override
				{
					return m_innerTask ? m_innerTask->canAbort() : false;
				}
				bool abort() override
				{
					return m_innerTask ? m_innerTask->abort() : Task::abort();
				}

			  private:
				ComponentPtr m_component;
				Net::Mode m_mode;
				Task::Ptr m_innerTask;
			};

			seq->addTask(makeShared<DeferredVersionListLoader>(component, netmode));
			loadTask = seq;
			loadTask->start();
			return LoadResult::RequiresRemote;
		}

		// Index is already synchronized, get version list directly
		auto versionList = component->getVersionList();
		if (!versionList)
		{
			qCWarning(instanceProfileResolveC) << "No version list found for" << component->m_uid;
			return LoadResult::Failed;
		}

		if (versionList->isLoaded())
		{
			return LoadResult::LoadedLocal;
		}

		if (netmode == Net::Mode::Offline)
		{
			return LoadResult::Failed;
		}

		loadTask = versionList->createLoadTask(netmode);

		if (loadTask)
		{
			loadTask->start();
			return LoadResult::RequiresRemote;
		}

		return LoadResult::Failed;
	}

} // namespace

void ComponentUpdateTask::loadComponents()
{
	LoadResult result		= LoadResult::LoadedLocal;
	size_t taskIndex		= 0;
	size_t componentIndex	= 0;
	d->remoteLoadSuccessful = true;

	// load all the components OR their lists...
	for (auto component : d->m_profile->d->components)
	{
		Task::Ptr loadTask;
		LoadResult singleResult = LoadResult::LoadedLocal;
		RemoteLoadStatus::Type loadType;
		component->resetComponentProblems();
		// Load version lists or components based on resolution mode
		switch (d->mode)
		{
			case Mode::Launch:
			{
				singleResult = loadComponent(component, loadTask, d->netmode);
				loadType	 = RemoteLoadStatus::Type::Version;
				break;
			}
			case Mode::Resolution:
			{
				singleResult = loadPackProfile(component, loadTask, d->netmode);
				loadType	 = RemoteLoadStatus::Type::List;
				break;
			}
		}
		if (singleResult == LoadResult::LoadedLocal)
		{
			component->updateCachedData();
		}
		result = composeLoadResult(result, singleResult);
		if (loadTask)
		{
			qCDebug(instanceProfileResolveC) << d->m_profile->d->m_instance->name() << "|"
											 << "Remote loading is being run for" << component->getName();
			connect(loadTask.get(), &Task::succeeded, this, [this, taskIndex]() { remoteLoadSucceeded(taskIndex); });
			connect(loadTask.get(),
					&Task::failed,
					this,
					[this, taskIndex](const QString& error) { remoteLoadFailed(taskIndex, error); });
			connect(loadTask.get(),
					&Task::aborted,
					this,
					[this, taskIndex]() { remoteLoadFailed(taskIndex, tr("Aborted")); });
			RemoteLoadStatus status;
			status.type				= loadType;
			status.PackProfileIndex = componentIndex;
			status.task				= loadTask;
			d->remoteLoadStatusList.append(status);
			taskIndex++;
		}
		componentIndex++;
	}
	d->remoteTasksInProgress = taskIndex;
	switch (result)
	{
		case LoadResult::LoadedLocal:
		{
			// Everything got loaded. Advance to dependency resolution.
			performUpdateActions();
			// In offline mode, only check dependencies; in online mode, resolve them
			resolveDependencies(d->netmode == Net::Mode::Offline);
			break;
		}
		case LoadResult::RequiresRemote:
		{
			// we wait for signals.
			break;
		}
		case LoadResult::Failed:
		{
			emitFailed(tr("Some component metadata load tasks failed."));
			break;
		}
	}
}

namespace
{
	struct RequireEx : public projt::meta::ComponentDependency
	{
		size_t indexOfFirstDependee = 0;
	};
	struct RequireCompositionResult
	{
		bool ok;
		RequireEx outcome;
	};
	using RequireExSet = std::set<RequireEx>;
} // namespace

static RequireCompositionResult composeRequirement(const RequireEx& a, const RequireEx& b)
{
	assert(a.uid == b.uid);
	RequireEx out;
	out.uid					 = a.uid;
	out.indexOfFirstDependee = std::min(a.indexOfFirstDependee, b.indexOfFirstDependee);
	if (a.equalsVersion.isEmpty())
	{
		out.equalsVersion = b.equalsVersion;
	}
	else if (b.equalsVersion.isEmpty())
	{
		out.equalsVersion = a.equalsVersion;
	}
	else if (a.equalsVersion == b.equalsVersion)
	{
		out.equalsVersion = a.equalsVersion;
	}
	else
	{
		// Version conflict: different exact versions required for same component
		qWarning() << "Version conflict for" << a.uid << ":" << a.equalsVersion << "vs" << b.equalsVersion;
		return { false, out };
	}

	if (a.suggests.isEmpty())
	{
		out.suggests = b.suggests;
	}
	else if (b.suggests.isEmpty())
	{
		out.suggests = a.suggests;
	}
	else
	{
		Version aVer(a.suggests);
		Version bVer(b.suggests);
		out.suggests = (aVer < bVer ? b.suggests : a.suggests);
	}
	return { true, out };
}

// gather the requirements from all components, finding any obvious conflicts
static bool gatherRequirementsFromComponents(const ComponentContainer& input, RequireExSet& output)
{
	bool succeeded		= true;
	size_t componentNum = 0;
	for (auto component : input)
	{
		auto& componentRequires = component->m_cachedRequires;
		for (const auto& componentRequire : componentRequires)
		{
			auto found = std::find_if(output.cbegin(),
									  output.cend(),
									  [componentRequire](const projt::meta::ComponentDependency& req)
									  { return req.uid == componentRequire.uid; });

			RequireEx componenRequireEx;
			componenRequireEx.uid				   = componentRequire.uid;
			componenRequireEx.suggests			   = componentRequire.suggests;
			componenRequireEx.equalsVersion		   = componentRequire.equalsVersion;
			componenRequireEx.indexOfFirstDependee = componentNum;

			if (found != output.cend())
			{
				// found... process it further
				auto result = composeRequirement(componenRequireEx, *found);
				if (result.ok)
				{
					output.erase(componenRequireEx);
					output.insert(result.outcome);
				}
				else
				{
					qCCritical(instanceProfileResolveC)
						<< "Conflicting requirements:" << componentRequire.uid
						<< "versions:" << componentRequire.equalsVersion << ";" << (*found).equalsVersion;
				}
				succeeded &= result.ok;
			}
			else
			{
				// not found, accumulate
				output.insert(componenRequireEx);
			}
		}
		componentNum++;
	}
	return succeeded;
}

/// Get list of uids that can be trivially removed because nothing is depending on them anymore (and they are installed as deps)
static void getTrivialRemovals(const ComponentContainer& components, const RequireExSet& reqs, QStringList& toRemove)
{
	for (const auto& component : components)
	{
		if (!component->m_dependencyOnly)
			continue;
		if (!component->m_cachedVolatile)
			continue;
		RequireEx reqNeedle;
		reqNeedle.uid	= component->m_uid;
		const auto iter = reqs.find(reqNeedle);
		if (iter == reqs.cend())
		{
			toRemove.append(component->m_uid);
		}
	}
}

/**
 * handles:
 * - trivial addition (there is an unmet requirement and it can be trivially met by adding something)
 * - trivial version conflict of dependencies == explicit version required and installed is different
 *
 * toAdd - set of requirements than mean adding a new component
 * toChange - set of requirements that mean changing version of an existing component
 */
static bool getTrivialComponentChanges(const ComponentIndex& index,
									   const RequireExSet& input,
									   RequireExSet& toAdd,
									   RequireExSet& toChange)
{
	enum class Decision
	{
		Undetermined,
		Met,
		Missing,
		VersionNotSame,
		LockedVersionNotSame
	} decision = Decision::Undetermined;

	QString reqStr;
	bool succeeded = true;
	// list the composed requirements and say if they are met or unmet
	for (auto& req : input)
	{
		do
		{
			if (req.equalsVersion.isEmpty())
			{
				reqStr = QString("Req: %1").arg(req.uid);
				if (index.contains(req.uid))
				{
					decision = Decision::Met;
				}
				else
				{
					toAdd.insert(req);
					decision = Decision::Missing;
				}
				break;
			}
			else
			{
				reqStr				 = QString("Req: %1 == %2").arg(req.uid, req.equalsVersion);
				const auto& compIter = index.find(req.uid);
				if (compIter == index.cend())
				{
					toAdd.insert(req);
					decision = Decision::Missing;
					break;
				}
				auto& comp = (*compIter);
				if (comp->getVersion() != req.equalsVersion)
				{
					if (comp->isCustom())
					{
						decision = Decision::LockedVersionNotSame;
					}
					else
					{
						if (comp->m_dependencyOnly)
						{
							decision = Decision::VersionNotSame;
						}
						else
						{
							decision = Decision::LockedVersionNotSame;
						}
					}
					break;
				}
				decision = Decision::Met;
			}
		}
		while (false);
		switch (decision)
		{
			case Decision::Undetermined:
				qCCritical(instanceProfileResolveC) << "No decision for" << reqStr;
				succeeded = false;
				break;
			case Decision::Met: qCDebug(instanceProfileResolveC) << reqStr << "Is met."; break;
			case Decision::Missing:
				qCDebug(instanceProfileResolveC)
					<< reqStr << "Is missing and should be added at" << req.indexOfFirstDependee;
				toAdd.insert(req);
				break;
			case Decision::VersionNotSame:
				qCDebug(instanceProfileResolveC) << reqStr << "already has different version that can be changed.";
				toChange.insert(req);
				break;
			case Decision::LockedVersionNotSame:
				qCDebug(instanceProfileResolveC) << reqStr << "already has different version that cannot be changed.";
				succeeded = false;
				break;
		}
	}
	return succeeded;
}

ComponentContainer ComponentUpdateTask::collectTreeLinked(const QString& uid)
{
	ComponentContainer linked;

	auto& components	 = d->m_profile->d->components;
	auto& componentIndex = d->m_profile->d->componentIndex;
	auto& instance		 = d->m_profile->d->m_instance;
	for (auto comp : components)
	{
		qCDebug(instanceProfileResolveC) << instance->name() << "|"
										 << "scanning" << comp->getID() << ":" << comp->getVersion() << "for tree link";
		auto dep = std::find_if(comp->m_cachedRequires.cbegin(),
								comp->m_cachedRequires.cend(),
								[uid](const projt::meta::ComponentDependency& req) -> bool { return req.uid == uid; });
		if (dep != comp->m_cachedRequires.cend())
		{
			qCDebug(instanceProfileResolveC)
				<< instance->name() << "|" << comp->getID() << ":" << comp->getVersion() << "depends on" << uid;
			linked.append(comp);
		}
	}
	auto iter = componentIndex.find(uid);
	if (iter != componentIndex.end())
	{
		ComponentPtr comp = *iter;
		comp->updateCachedData();
		qCDebug(instanceProfileResolveC) << instance->name() << "|" << comp->getID() << ":" << comp->getVersion()
										 << "has" << comp->m_cachedRequires.size() << "dependencies";
		for (auto dep : comp->m_cachedRequires)
		{
			qCDebug(instanceProfileC) << instance->name() << "|" << uid << "depends on" << dep.uid;
			auto found = componentIndex.find(dep.uid);
			if (found != componentIndex.end())
			{
				qCDebug(instanceProfileC) << instance->name() << "|" << (*found)->getID() << "is present";
				linked.append(*found);
			}
		}
	}
	return linked;
}

// Architecture Note: This method directly manipulates PackProfile internals.
// Proper abstraction would require richer data types (dependency graph, version constraints),
// but current implementation is sufficient for the launcher's use case.
// A full rewrite to use a proper dependency graph (like SAT solving) is out of scope for now.
void ComponentUpdateTask::resolveDependencies(bool checkOnly)
{
	qCDebug(instanceProfileResolveC) << "Resolving dependencies";
	/*
	 * this is a naive dependency resolving algorithm. all it does is check for following conditions and react in simple
	 * ways:
	 * 1. There are conflicting dependencies on the same uid with different exact version numbers
	 *    -> hard error
	 * 2. A dependency has non-matching exact version number
	 *    -> hard error
	 * 3. A dependency is entirely missing and needs to be injected before the dependee(s)
	 *    -> requirements are injected
	 *
	 * NOTE: this is a placeholder and should eventually be replaced with something 'serious'
	 */
	auto& components	 = d->m_profile->d->components;
	auto& componentIndex = d->m_profile->d->componentIndex;

	RequireExSet allRequires;
	QStringList toRemove;
	do
	{
		allRequires.clear();
		toRemove.clear();
		if (!gatherRequirementsFromComponents(components, allRequires))
		{
			finalizeComponents();
			emitFailed(tr("Conflicting requirements detected during dependency checking!"));
			return;
		}
		getTrivialRemovals(components, allRequires, toRemove);
		if (!toRemove.isEmpty())
		{
			qCDebug(instanceProfileResolveC) << "Removing obsolete components...";
			for (auto& remove : toRemove)
			{
				qCDebug(instanceProfileResolveC) << "Removing" << remove;
				d->m_profile->remove(remove);
			}
		}
	}
	while (!toRemove.isEmpty());
	RequireExSet toAdd;
	RequireExSet toChange;
	bool succeeded = getTrivialComponentChanges(componentIndex, allRequires, toAdd, toChange);
	if (!succeeded)
	{
		finalizeComponents();
		emitFailed(tr("Instance has conflicting dependencies."));
		return;
	}
	if (checkOnly)
	{
		finalizeComponents();
		if (toAdd.size() || toChange.size())
		{
			emitFailed(tr("Instance has unresolved dependencies while loading/checking for launch."));
		}
		else
		{
			emitSucceeded();
		}
		return;
	}

	bool recursionNeeded = false;
	if (toAdd.size())
	{
		// add stuff...
		for (auto& add : toAdd)
		{
			auto component = makeShared<Component>(d->m_profile, add.uid);
			if (!add.equalsVersion.isEmpty())
			{
				// exact version
				qCDebug(instanceProfileResolveC) << "Adding" << add.uid << "version" << add.equalsVersion
												 << "at position" << add.indexOfFirstDependee;
				component->m_version = add.equalsVersion;
			}
			else
			{
				// version needs to be decided
				qCDebug(instanceProfileResolveC) << "Adding" << add.uid << "at position" << add.indexOfFirstDependee;
				if (!add.suggests.isEmpty())
				{
					// Use suggested version if available
					component->m_version = add.suggests;
				}
				else
				{
					// Intermediary mappings must align with the selected Minecraft version.
					// Prefer a metadata entry that explicitly depends on that MC version.
					if (add.uid == "net.fabricmc.intermediary" || add.uid == "org.quiltmc.hashed")
					{
						auto minecraft =
							std::find_if(components.begin(),
										 components.end(),
										 [](ComponentPtr& cmp) { return cmp->getID() == "net.minecraft"; });
						if (minecraft != components.end())
						{
							const auto minecraftVersion = (*minecraft)->getVersion();
							auto versionList			= APPLICATION->metadataIndex()->component(add.uid);
							if (versionList)
							{
								versionList->waitUntilReady();
								auto matched = versionList->stableForParent("net.minecraft", minecraftVersion);
								if (!matched)
								{
									matched = versionList->latestForParent("net.minecraft", minecraftVersion);
								}
								if (matched)
								{
									component->m_version = matched->descriptor();
								}
							}
							if (component->m_version.isEmpty())
							{
								component->m_version = minecraftVersion;
							}
						}
					}

					// Try to get recommended version from metadata
					if (component->m_version.isEmpty())
					{
						auto versionList = APPLICATION->metadataIndex()->component(add.uid);
						if (versionList)
						{
							versionList->waitUntilReady();
							auto recommended = versionList->getRecommended();
							if (recommended)
							{
								component->m_version = recommended->descriptor();
							}
						}
					}

					// Last resort: known defaults for LWJGL when metadata unavailable
					if (component->m_version.isEmpty())
					{
						if (add.uid == "org.lwjgl")
						{
							component->m_version = "2.9.1";
						}
						else if (add.uid == "org.lwjgl3")
						{
							component->m_version = "3.1.2";
						}
					}
				}
			}
			component->m_dependencyOnly = true;
			// Direct insertion to component list is intentional - this is part of dependency resolution
			// which requires atomic updates to the profile structure.
			d->m_profile->insertComponent(add.indexOfFirstDependee, component);
			componentIndex[add.uid] = component;
		}
		recursionNeeded = true;
	}
	if (toChange.size())
	{
		// Version changes during dependency resolution require direct component access
		// as the dependency resolver may adjust versions to satisfy constraints.
		for (auto& change : toChange)
		{
			qCDebug(instanceProfileResolveC) << "Setting version of " << change.uid << "to" << change.equalsVersion;
			auto component = componentIndex[change.uid];
			component->setVersion(change.equalsVersion);
		}
		recursionNeeded = true;
	}

	if (recursionNeeded)
	{
		loadComponents();
	}
	else
	{
		finalizeComponents();
		emitSucceeded();
	}
}

// Variant visitation via lambda
template <class... Ts>
struct overload : Ts...
{
	using Ts::operator()...;
};
template <class... Ts>
overload(Ts...) -> overload<Ts...>;

void ComponentUpdateTask::performUpdateActions()
{
	auto& instance = d->m_profile->d->m_instance;
	bool addedActions;
	QStringList toRemove;
	do
	{
		addedActions = false;
		toRemove.clear();
		auto& components	 = d->m_profile->d->components;
		auto& componentIndex = d->m_profile->d->componentIndex;
		for (auto component : components)
		{
			if (!component)
			{
				continue;
			}
			auto action	 = component->getUpdateAction();
			auto visitor = overload{
				[](const UpdateActionNone&)
				{
					// noop
				},
				[&component, &instance](const UpdateActionChangeVersion& cv)
				{
					qCDebug(instanceProfileResolveC) << instance->name() << "|"
													 << "UpdateActionChangeVersion" << component->getID() << ":"
													 << component->getVersion() << "change to" << cv.targetVersion;
					component->setVersion(cv.targetVersion);
					component->waitLoadMeta();
				},
				[&component, &instance](const UpdateActionLatestRecommendedCompatible& lrc)
				{
					qCDebug(instanceProfileResolveC)
						<< instance->name() << "|"
						<< "UpdateActionLatestRecommendedCompatible" << component->getID() << ":"
						<< component->getVersion() << "updating to latest recommend or compatible with" << lrc.parentUid
						<< lrc.version;
					auto versionList = APPLICATION->metadataIndex()->component(component->getID());
					if (versionList)
					{
						versionList->waitUntilReady();
						auto recommended = versionList->stableForParent(lrc.parentUid, lrc.version);
						if (!recommended)
						{
							recommended = versionList->latestForParent(lrc.parentUid, lrc.version);
						}
						if (recommended)
						{
							component->setVersion(recommended->versionId());
							component->waitLoadMeta();
							return;
						}
						else
						{
							component->addComponentProblem(ProblemSeverity::Error,
														   QObject::tr("No compatible version of %1 found for %2 %3")
															   .arg(component->getName(), lrc.parentName, lrc.version));
						}
					}
					else
					{
						component->addComponentProblem(
							ProblemSeverity::Error,
							QObject::tr("No version list in metadata index for %1").arg(component->getID()));
					}
				},
				[&component, &instance, &toRemove](const UpdateActionRemove&)
				{
					qCDebug(instanceProfileResolveC)
						<< instance->name() << "|"
						<< "UpdateActionRemove" << component->getID() << ":" << component->getVersion() << "removing";
					toRemove.append(component->getID());
				},
				[this, &component, &instance, &addedActions, &componentIndex](const UpdateActionImportantChanged& ic)
				{
					qCDebug(instanceProfileResolveC)
						<< instance->name() << "|"
						<< "UpdateImportantChanged" << component->getID() << ":" << component->getVersion()
						<< "was changed from" << ic.oldVersion << "updating linked components";
					auto oldVersion =
						APPLICATION->metadataIndex()->loadVersionBlocking(component->getID(), ic.oldVersion);
					for (auto oldReq : oldVersion->dependencies())
					{
						auto currentlyRequired = component->m_cachedRequires.find(oldReq);
						if (currentlyRequired == component->m_cachedRequires.cend())
						{
							auto oldReqComp = componentIndex.find(oldReq.uid);
							if (oldReqComp != componentIndex.cend())
							{
								(*oldReqComp)->setUpdateAction(UpdateAction{ UpdateActionRemove{} });
								addedActions = true;
							}
						}
					}
					auto linked = collectTreeLinked(component->getID());
					for (auto comp : linked)
					{
						if (comp->isCustom())
						{
							continue;
						}
						auto compUid   = comp->getID();
						auto parentReq = std::find_if(component->m_cachedRequires.begin(),
													  component->m_cachedRequires.end(),
													  [compUid](const projt::meta::ComponentDependency& req)
													  { return req.uid == compUid; });
						if (parentReq != component->m_cachedRequires.end())
						{
							auto newVersion =
								parentReq->equalsVersion.isEmpty() ? parentReq->suggests : parentReq->equalsVersion;
							if (!newVersion.isEmpty())
							{
								comp->setUpdateAction(UpdateAction{ UpdateActionChangeVersion{ newVersion } });
							}
							else
							{
								comp->setUpdateAction(UpdateAction{ UpdateActionLatestRecommendedCompatible{
									component->getID(),
									component->getName(),
									component->getVersion(),
								} });
							}
						}
						else
						{
							comp->setUpdateAction(UpdateAction{ UpdateActionLatestRecommendedCompatible{
								component->getID(),
								component->getName(),
								component->getVersion(),
							} });
						}
						addedActions = true;
					}
				}
			};
			std::visit(visitor, action);
			component->clearUpdateAction();
			for (auto uid : toRemove)
			{
				d->m_profile->remove(uid);
			}
		}
	}
	while (addedActions);
}

void ComponentUpdateTask::finalizeComponents()
{
	auto& components	 = d->m_profile->d->components;
	auto& componentIndex = d->m_profile->d->componentIndex;
	for (auto component : components)
	{
		for (auto req : component->m_cachedRequires)
		{
			auto found = componentIndex.find(req.uid);
			if (found == componentIndex.cend())
			{
				component->addComponentProblem(
					ProblemSeverity::Error,
					QObject::tr("%1 is missing requirement %2 %3")
						.arg(component->getName(),
							 req.uid,
							 req.equalsVersion.isEmpty() ? req.suggests : req.equalsVersion));
			}
			else
			{
				auto reqComp = *found;
				if (!reqComp->getProblems().isEmpty())
				{
					component->addComponentProblem(
						reqComp->getProblemSeverity(),
						QObject::tr("%1, a dependency of this component, has reported issues").arg(reqComp->getName()));
				}
				if (!req.equalsVersion.isEmpty() && req.equalsVersion != reqComp->getVersion())
				{
					component->addComponentProblem(
						ProblemSeverity::Error,
						QObject::tr("%1, a dependency of this component, is not the required version %2")
							.arg(reqComp->getName(), req.equalsVersion));
				}
				else if (!req.suggests.isEmpty() && req.suggests != reqComp->getVersion())
				{
					component->addComponentProblem(
						ProblemSeverity::Warning,
						QObject::tr("%1, a dependency of this component, is not the suggested version %2")
							.arg(reqComp->getName(), req.suggests));
				}
			}
		}
		for (auto conflict : component->knownConflictingComponents())
		{
			auto found = componentIndex.find(conflict);
			if (found != componentIndex.cend())
			{
				auto foundComp = *found;
				if (foundComp->isCustom())
				{
					continue;
				}
				component->addComponentProblem(
					ProblemSeverity::Warning,
					QObject::tr("%1 and %2 are known to not work together. It is recommended to remove one of them.")
						.arg(component->getName(), foundComp->getName()));
			}
		}
	}
}

void ComponentUpdateTask::remoteLoadSucceeded(size_t taskIndex)
{
	if (static_cast<size_t>(d->remoteLoadStatusList.size()) < taskIndex)
	{
		qCWarning(instanceProfileResolveC) << "Got task index outside of results" << taskIndex;
		return;
	}
	auto& taskSlot = d->remoteLoadStatusList[taskIndex];
	disconnect(taskSlot.task.get(), &Task::succeeded, this, nullptr);
	disconnect(taskSlot.task.get(), &Task::failed, this, nullptr);
	disconnect(taskSlot.task.get(), &Task::aborted, this, nullptr);
	if (taskSlot.finished)
	{
		qCWarning(instanceProfileResolveC) << "Got multiple results from remote load task" << taskIndex;
		return;
	}
	qCDebug(instanceProfileResolveC) << "Remote task" << taskIndex << "succeeded";
	taskSlot.succeeded = true;
	taskSlot.finished  = true;
	d->remoteTasksInProgress--;
	// update the cached data of the component from the downloaded version file.
	if (taskSlot.type == RemoteLoadStatus::Type::Version)
	{
		auto component		= d->m_profile->getComponent(taskSlot.PackProfileIndex);
		component->m_loaded = true;
		component->updateCachedData();
	}
	checkIfAllFinished();
}

void ComponentUpdateTask::remoteLoadFailed(size_t taskIndex, const QString& msg)
{
	if (static_cast<size_t>(d->remoteLoadStatusList.size()) < taskIndex)
	{
		qCWarning(instanceProfileResolveC) << "Got task index outside of results" << taskIndex;
		return;
	}
	auto& taskSlot = d->remoteLoadStatusList[taskIndex];
	disconnect(taskSlot.task.get(), &Task::succeeded, this, nullptr);
	disconnect(taskSlot.task.get(), &Task::failed, this, nullptr);
	disconnect(taskSlot.task.get(), &Task::aborted, this, nullptr);
	if (taskSlot.finished)
	{
		qCWarning(instanceProfileResolveC) << "Got multiple results from remote load task" << taskIndex;
		return;
	}
	qCDebug(instanceProfileResolveC) << "Remote task" << taskIndex << "failed: " << msg;
	d->remoteLoadSuccessful = false;
	taskSlot.succeeded		= false;
	taskSlot.finished		= true;
	d->remoteTasksInProgress--;
	checkIfAllFinished();
}

void ComponentUpdateTask::checkIfAllFinished()
{
	if (d->remoteTasksInProgress)
	{
		// not yet...
		return;
	}
	if (d->remoteLoadSuccessful)
	{
		// nothing bad happened... clear the temp load status and proceed with looking at dependencies
		d->remoteLoadStatusList.clear();
		performUpdateActions();
		// In online mode, resolve dependencies (add missing components)
		// In offline mode, only check (no network to download new components)
		resolveDependencies(d->netmode == Net::Mode::Offline);
	}
	else
	{
		// remote load failed... report error and bail
		QStringList allErrorsList;
		for (auto& item : d->remoteLoadStatusList)
		{
			if (!item.succeeded)
			{
				const ComponentPtr component = d->m_profile->getComponent(item.PackProfileIndex);
				allErrorsList.append(
					tr("Could not download metadata for %1 %2. Please change the version or try again later.")
						.arg(component->getName(), component->m_version));
			}
		}
		auto allErrors = allErrorsList.join("\n");
		emitFailed(
			tr("Component metadata update task failed while downloading from remote server:\n%1").arg(allErrors));
		d->remoteLoadStatusList.clear();
	}
}
