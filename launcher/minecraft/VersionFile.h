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
#include <QHash>
#include <QList>
#include <QSet>
#include <QString>
#include <QStringList>

#include <meta/JsonFormat.hpp>
#include <memory>
#include "Agent.h"
#include "Library.h"
#include "ProblemProvider.h"
#include "java/core/RuntimePackage.hpp"
#include "minecraft/Rule.h"

class PackProfile;
class VersionFile;
class LaunchProfile;
struct MojangDownloadInfo;
struct MojangAssetIndexInfo;

using VersionFilePtr = std::shared_ptr<VersionFile>;
class VersionFile : public ProblemContainer
{
	friend class MojangVersionFormat;
	friend class OneSixVersionFormat;

  public: /* methods */
	void applyTo(LaunchProfile* profile, const RuntimeContext& runtimeContext);

  public: /* data */
	/// ProjT Launcher: order hint for this version file if no explicit order is set
	int order = 0;

	/// ProjT Launcher: human readable name of this package
	QString name;

	/// ProjT Launcher: package ID of this package
	QString uid;

	/// ProjT Launcher: version of this package
	QString version;

	/// ProjT Launcher: DEPRECATED dependency on a Minecraft version
	QString dependsOnMinecraftVersion;

	/// Mojang: DEPRECATED used to version the Mojang version format
	int minimumLauncherVersion = -1;

	/// Mojang: DEPRECATED version of Minecraft this is
	QString minecraftVersion;

	/// Mojang: class to launch Minecraft with
	QString mainClass;

	/// ProjT Launcher: class to launch legacy Minecraft with (embed in a custom window)
	QString appletClass;

	/// Mojang: Minecraft launch arguments (may contain placeholders for variable substitution)
	QString minecraftArguments;

	/// ProjT Launcher: Additional JVM launch arguments
	QStringList addnJvmArguments;

	/// Mojang: list of compatible java majors
	QList<int> compatibleJavaMajors;

	/// Mojang: the name of recommended java version
	QString compatibleJavaName;

	/// Mojang: type of the Minecraft version
	QString type;

	/// Mojang: the time this version was actually released by Mojang
	QDateTime releaseTime;

	/// Mojang: DEPRECATED the time this version was last updated by Mojang
	QDateTime updateTime;

	/// Mojang: DEPRECATED asset group to be used with Minecraft
	QString assets;

	/// ProjT Launcher: list of tweaker mod arguments for launchwrapper
	QStringList addTweakers;

	/// Mojang: list of libraries to add to the version
	QList<LibraryPtr> libraries;

	/// ProjT Launcher: list of maven files to put in the libraries folder, but not in classpath
	QList<LibraryPtr> mavenFiles;

	/// ProjT Launcher: list of agents to add to JVM arguments
	QList<AgentPtr> agents;

	/// The main jar (Minecraft version library, normally)
	LibraryPtr mainJar;

	/// ProjT Launcher: list of attached traits of this version file - used to enable features
	QSet<QString> traits;

	/// ProjT Launcher: list of jar mods added to this version
	QList<LibraryPtr> jarMods;

	/// ProjT Launcher: list of mods added to this version
	QList<LibraryPtr> mods;

	/**
	 * ProjT Launcher: set of packages this depends on
	 * NOTE: this is shared with the meta format!!!
	 */
	projt::meta::DependencySet m_requires;

	/**
	 * ProjT Launcher: set of packages this conflicts with
	 * NOTE: this is shared with the meta format!!!
	 */
	projt::meta::DependencySet conflicts;

	/// is volatile -- may be removed as soon as it is no longer needed by something else
	bool m_volatile = false;

	QList<projt::java::RuntimePackagePtr> runtimes;

  public:
	// Mojang: DEPRECATED list of 'downloads' - client jar, server jar, windows server exe, maybe more.
	QMap<QString, std::shared_ptr<MojangDownloadInfo>> mojangDownloads;

	// Mojang: extended asset index download information
	std::shared_ptr<MojangAssetIndexInfo> mojangAssetIndex;
};
