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
#pragma once

#include <QDateTime>
#include <QJsonDocument>
#include <QList>
#include <memory>
#include <optional>
#include <variant>
#include "ProblemProvider.h"
#include "QObjectPtr.h"
#include "meta/JsonFormat.hpp"
#include "modplatform/ModIndex.h"

class PackProfile;
class LaunchProfile;
namespace projt::meta
{
	class MetaVersion;
	class MetaVersionList;
} // namespace projt::meta
class VersionFile;

struct UpdateActionChangeVersion
{
	/// version to change to
	QString targetVersion;
};
struct UpdateActionLatestRecommendedCompatible
{
	/// Parent uid
	QString parentUid;
	QString parentName;
	/// Parent version
	QString version;
	///
};
struct UpdateActionRemove
{};
struct UpdateActionImportantChanged
{
	QString oldVersion;
};

using UpdateActionNone = std::monostate;

using UpdateAction = std::variant<UpdateActionNone,
								  UpdateActionChangeVersion,
								  UpdateActionLatestRecommendedCompatible,
								  UpdateActionRemove,
								  UpdateActionImportantChanged>;

struct ModloaderMapEntry
{
	ModPlatform::ModLoaderType type;
	QStringList knownConflictingComponents;
};

class Component : public QObject, public ProblemProvider
{
	Q_OBJECT
  public:
	Component(PackProfile* parent, const QString& uid);

	// DEPRECATED: remove these constructors?
	Component(PackProfile* parent, const QString& uid, std::shared_ptr<VersionFile> file);

	virtual ~Component()
	{}

	static const QMap<QString, ModloaderMapEntry> KNOWN_MODLOADERS;

	void applyTo(LaunchProfile* profile);

	bool isEnabled();
	bool setEnabled(bool state);
	bool canBeDisabled();

	bool isMoveable();
	bool isCustomizable();
	bool isRevertible();
	bool isRemovable();
	bool isCustom();
	bool isVersionChangeable(bool wait = true);
	bool isKnownModloader();
	QStringList knownConflictingComponents();

	// DEPRECATED: explicit numeric order values, used for loading old non-component config.
	// NOTE: Kept for legacy migration support.
	void setOrder(int order);
	int getOrder();

	QString getID();
	QString getName();
	QString getVersion();
	std::shared_ptr<projt::meta::MetaVersion> getMeta();
	QDateTime getReleaseDateTime();

	QString getFilename();

	std::shared_ptr<class VersionFile> getVersionFile() const;
	std::shared_ptr<projt::meta::MetaVersionList> getVersionList() const;

	void setImportant(bool state);

	const QList<PatchProblem> getProblems() const override;
	ProblemSeverity getProblemSeverity() const override;
	void addComponentProblem(ProblemSeverity severity, const QString& description);
	void resetComponentProblems();

	void setVersion(const QString& version);
	bool customize();
	bool revert();

	void updateCachedData();

	void waitLoadMeta();

	void setUpdateAction(const UpdateAction& action);
	void clearUpdateAction();
	UpdateAction getUpdateAction();

  signals:
	void dataChanged();

  public: /* data */
	PackProfile* m_parent;

	// BEGIN: persistent component list properties
	/// ID of the component
	QString m_uid;
	/// version of the component - when there's a custom json override, this is also the version the component reverts to
	QString m_version;
	/// if true, this has been added automatically to satisfy dependencies and may be automatically removed
	bool m_dependencyOnly = false;
	/// if true, the component is either the main component of the instance, or otherwise important and cannot be removed.
	bool m_important = false;
	/// if true, the component is disabled
	bool m_disabled = false;

	/// cached name for display purposes, taken from the version file (meta or local override)
	QString m_cachedName;
	/// cached version for display AND other purposes, taken from the version file (meta or local override)
	QString m_cachedVersion;
	/// cached set of requirements, taken from the version file (meta or local override)
	projt::meta::DependencySet m_cachedRequires;
	projt::meta::DependencySet m_cachedConflicts;
	/// if true, the component is volatile and may be automatically removed when no longer needed
	bool m_cachedVolatile = false;
	// END: persistent component list properties

	// DEPRECATED: explicit numeric order values, used for loading old non-component config.
	// NOTE: Kept for legacy migration support.
	bool m_orderOverride = false;
	int m_order			 = 0;

	// load state
	std::shared_ptr<projt::meta::MetaVersion> m_metaVersion;
	std::shared_ptr<VersionFile> m_file;
	bool m_loaded = false;

  private:
	QList<PatchProblem> m_componentProblems;
	ProblemSeverity m_componentProblemSeverity = ProblemSeverity::None;
	UpdateAction m_updateAction				   = UpdateAction{ UpdateActionNone{} };
};

using ComponentPtr = shared_qobject_ptr<Component>;
