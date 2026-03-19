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

#include "AutoInstallJava.hpp"
#include <QDir>
#include <QFileInfo>
#include <memory>

#include "Application.h"
#include "FileSystem.h"
#include "MessageLevel.h"
#include "QObjectPtr.h"
#include "SysInfo.h"
#include "java/core/RuntimeInstall.hpp"
#include "java/core/RuntimeVersion.hpp"
#include "java/services/RuntimeCatalog.hpp"
#include "java/services/RuntimeScanner.hpp"
#include "java/download/RuntimeArchiveTask.hpp"
#include "java/download/RuntimeManifestTask.hpp"
#include "java/download/RuntimeLinkTask.hpp"
#include "meta/Index.hpp"
#include "minecraft/MinecraftInstance.h"
#include "minecraft/PackProfile.h"
#include "net/Mode.h"
#include "tasks/SequentialTask.h"

AutoInstallJava::AutoInstallJava(projt::launch::LaunchPipeline* parent)
	: projt::launch::LaunchStage(parent),
	  m_instance(m_flow->instance()),
	  m_supported_arch(SysInfo::getSupportedJavaArchitecture()) {};

void AutoInstallJava::executeTask()
{
	auto settings = m_instance->settings();
	if (!APPLICATION->settings()->get("AutomaticJavaSwitch").toBool())
	{
		emitSucceeded();
		return;
	}
	if (!settings->get("OverrideJavaLocation").toBool())
	{
		emitSucceeded();
		return;
	}
	auto packProfile			   = m_instance->getPackProfile();
	const auto configuredJavaPath  = settings->get("JavaPath").toString();
	const bool hasValidJava		   = !FS::ResolveExecutable(configuredJavaPath).isEmpty();
	const bool ignoreCompatibility = settings->get("IgnoreJavaCompatibility").toBool();
	const auto compatibleMajors	   = packProfile->getProfile()->getCompatibleJavaMajors();
	bool compatibleJava			   = true;
	if (!ignoreCompatibility && !compatibleMajors.isEmpty())
	{
		auto storedVersion = settings->get("JavaVersion").toString();
		if (storedVersion.isEmpty())
		{
			compatibleJava = false;
		}
		else
		{
			projt::java::RuntimeVersion javaVersion(storedVersion);
			compatibleJava = compatibleMajors.contains(javaVersion.major());
		}
	}
	if (hasValidJava && compatibleJava)
	{
		emitSucceeded();
		return;
	}
	if (!APPLICATION->settings()->get("AutomaticJavaDownload").toBool())
	{
		auto javas	   = APPLICATION->runtimeCatalog();
		m_current_task = javas->getLoadTask();
		connect(
			m_current_task.get(),
			&Task::finished,
			this,
			[this, javas, packProfile]
			{
				for (auto i = 0; i < javas->count(); i++)
				{
					auto java = std::dynamic_pointer_cast<projt::java::RuntimeInstall>(javas->at(i));
					if (java && packProfile->getProfile()->getCompatibleJavaMajors().contains(java->version.major()))
					{
						if (!java->is_64bit)
						{
							emit logLine(tr("The automatic Java mechanism detected a 32-bit installation of Java."),
										 MessageLevel::Launcher);
						}
						setJavaPath(java->path);
						return;
					}
				}
				emit logLine(tr("No compatible Java version was found. Using the default one."), MessageLevel::Warning);
				emitSucceeded();
			});
		connect(m_current_task.get(), &Task::progress, this, &AutoInstallJava::setProgress);
		connect(m_current_task.get(), &Task::stepProgress, this, &AutoInstallJava::propagateStepProgress);
		connect(m_current_task.get(), &Task::status, this, &AutoInstallJava::setStatus);
		connect(m_current_task.get(), &Task::details, this, &AutoInstallJava::setDetails);
		emit progressReportingRequest();
		return;
	}
	if (m_supported_arch.isEmpty())
	{
		emit logLine(
			tr("Your system (%1-%2) is not compatible with automatic Java installation. Using the default Java path.")
				.arg(SysInfo::currentSystem(), SysInfo::useQTForArch()),
			MessageLevel::Warning);
		emitSucceeded();
		return;
	}
	auto wantedJavaName = packProfile->getProfile()->getCompatibleJavaName();
	if (wantedJavaName.isEmpty())
	{
		emit logLine(tr("Your meta information is out of date or doesn't have the information necessary to determine "
						"what installation of "
						"Java should be used. "
						"Using the default Java path."),
					 MessageLevel::Warning);
		emitSucceeded();
		return;
	}
	QDir javaDir(APPLICATION->javaPath());
	auto wantedJavaPath = javaDir.absoluteFilePath(wantedJavaName);
	if (QFileInfo::exists(wantedJavaPath))
	{
		setJavaPathFromPartial();
		return;
	}
	auto versionList = APPLICATION->metadataIndex()->component("net.minecraft.java");
	m_current_task	 = versionList->getLoadTask();
	connect(m_current_task.get(), &Task::succeeded, this, &AutoInstallJava::tryNextMajorJava);
	connect(m_current_task.get(), &Task::failed, this, &AutoInstallJava::emitFailed);
	connect(m_current_task.get(), &Task::progress, this, &AutoInstallJava::setProgress);
	connect(m_current_task.get(), &Task::stepProgress, this, &AutoInstallJava::propagateStepProgress);
	connect(m_current_task.get(), &Task::status, this, &AutoInstallJava::setStatus);
	connect(m_current_task.get(), &Task::details, this, &AutoInstallJava::setDetails);
	if (!m_current_task->isRunning())
	{
		m_current_task->start();
	}
	emit progressReportingRequest();
}

void AutoInstallJava::setJavaPath(QString path)
{
	auto settings = m_instance->settings();
	settings->set("OverrideJavaLocation", true);
	settings->set("JavaPath", path);
	settings->set("AutomaticJava", true);
	emit logLine(tr("Compatible Java found at: %1.").arg(path), MessageLevel::Launcher);
	emitSucceeded();
}

void AutoInstallJava::setJavaPathFromPartial()
{
	auto packProfile = m_instance->getPackProfile();
	auto javaName	 = packProfile->getProfile()->getCompatibleJavaName();
	QDir javaDir(APPLICATION->javaPath());
	// just checking if the executable is there should suffice
	// but if needed this can be achieved through refreshing the javalist
	// and retrieving the path that contains the java name
	auto relativeBinary = FS::PathCombine(javaName, "bin", projt::java::RuntimeScanner::executableName());
	auto finalPath		= javaDir.absoluteFilePath(relativeBinary);
	if (QFileInfo::exists(finalPath))
	{
		setJavaPath(finalPath);
	}
	else
	{
		emit logLine(
			tr("No compatible Java version was found (the binary file does not exist). Using the default one."),
			MessageLevel::Warning);
		emitSucceeded();
	}
	return;
}

void AutoInstallJava::downloadJava(projt::meta::MetaVersion::Ptr version, QString javaName)
{
	auto runtimes = version->detailedData()->runtimes;
	for (auto java : runtimes)
	{
		if (java->runtimeOS == m_supported_arch && java->name() == javaName)
		{
			QDir javaDir(APPLICATION->javaPath());
			auto final_path = javaDir.absoluteFilePath(java->displayName);
			switch (java->downloadKind)
			{
				case projt::java::PackageKind::Manifest:
					m_current_task = makeShared<projt::java::RuntimeManifestTask>(java->url,
																				  final_path,
																				  java->checksumType,
																				  java->checksumHash);
					break;
				case projt::java::PackageKind::Archive:
					m_current_task = makeShared<projt::java::RuntimeArchiveTask>(java->url,
																				 final_path,
																				 java->checksumType,
																				 java->checksumHash);
					break;
				case projt::java::PackageKind::Unknown:
					emitFailed(tr("Could not determine Java download type!"));
					return;
			}
#if defined(Q_OS_MACOS)
			auto seq = makeShared<SequentialTask>(tr("Install Java"));
			seq->addTask(m_current_task);
			seq->addTask(makeShared<projt::java::RuntimeLinkTask>(final_path));
			m_current_task = seq;
#endif
			auto deletePath = [final_path] { FS::deletePath(final_path); };
			connect(m_current_task.get(),
					&Task::failed,
					this,
					[this, deletePath](QString reason)
					{
						deletePath();
						emitFailed(reason);
					});
			connect(m_current_task.get(), &Task::aborted, this, [deletePath] { deletePath(); });
			connect(m_current_task.get(), &Task::succeeded, this, &AutoInstallJava::setJavaPathFromPartial);
			connect(m_current_task.get(), &Task::failed, this, &AutoInstallJava::tryNextMajorJava);
			connect(m_current_task.get(), &Task::progress, this, &AutoInstallJava::setProgress);
			connect(m_current_task.get(), &Task::stepProgress, this, &AutoInstallJava::propagateStepProgress);
			connect(m_current_task.get(), &Task::status, this, &AutoInstallJava::setStatus);
			connect(m_current_task.get(), &Task::details, this, &AutoInstallJava::setDetails);
			m_current_task->start();
			return;
		}
	}
	tryNextMajorJava();
}

void AutoInstallJava::tryNextMajorJava()
{
	if (!isRunning())
		return;
	auto versionList	   = APPLICATION->metadataIndex()->component("net.minecraft.java");
	auto packProfile	   = m_instance->getPackProfile();
	auto wantedJavaName	   = packProfile->getProfile()->getCompatibleJavaName();
	auto majorJavaVersions = packProfile->getProfile()->getCompatibleJavaMajors();
	if (m_majorJavaVersionIndex >= majorJavaVersions.length())
	{
		emit logLine(tr("No versions of Java were found for your operating system: %1-%2")
						 .arg(SysInfo::currentSystem(), SysInfo::useQTForArch()),
					 MessageLevel::Warning);
		emit logLine(tr("No compatible version of Java was found. Using the default one."), MessageLevel::Warning);
		emitSucceeded();
		return;
	}
	auto majorJavaVersion = majorJavaVersions[m_majorJavaVersionIndex];
	m_majorJavaVersionIndex++;

	auto javaMajor = versionList->getMetaVersion(QString("java%1").arg(majorJavaVersion));
	if (!javaMajor)
	{
		tryNextMajorJava();
		return;
	}

	if (javaMajor->isFullyLoaded())
	{
		downloadJava(javaMajor, wantedJavaName);
	}
	else
	{
		m_current_task = APPLICATION->metadataIndex()->loadVersionTask("net.minecraft.java",
																	   javaMajor->versionId(),
																	   Net::Mode::Online);
		connect(m_current_task.get(),
				&Task::succeeded,
				this,
				[this, javaMajor, wantedJavaName] { downloadJava(javaMajor, wantedJavaName); });
		connect(m_current_task.get(), &Task::failed, this, &AutoInstallJava::tryNextMajorJava);
		connect(m_current_task.get(), &Task::progress, this, &AutoInstallJava::setProgress);
		connect(m_current_task.get(), &Task::stepProgress, this, &AutoInstallJava::propagateStepProgress);
		connect(m_current_task.get(), &Task::status, this, &AutoInstallJava::setStatus);
		connect(m_current_task.get(), &Task::details, this, &AutoInstallJava::setDetails);
		if (!m_current_task->isRunning())
		{
			m_current_task->start();
		}
	}
}
bool AutoInstallJava::abort()
{
	if (m_current_task && m_current_task->canAbort())
	{
		auto status = m_current_task->abort();
		emitAborted();
		return status;
	}
	return Task::abort();
}
