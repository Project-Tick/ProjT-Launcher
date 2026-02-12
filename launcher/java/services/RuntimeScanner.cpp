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

#include "java/services/RuntimeScanner.hpp"

#include <QDir>
#include <QFileInfo>
#include <QProcessEnvironment>
#include <QStandardPaths>
#include <functional>
#include <vector>

#include "Application.h"
#include "FileSystem.h"

namespace projt::java
{
	static QStringList resolveExecutables(const QStringList& candidates)
	{
		QStringList resolved;
		resolved.reserve(candidates.size());
		for (const auto& candidate : candidates)
		{
			if (candidate.isEmpty())
			{
				continue;
			}
			if (QDir::isRelativePath(candidate))
			{
				const auto found = QStandardPaths::findExecutable(candidate);
				if (!found.isEmpty())
				{
					resolved.append(QFileInfo(found).canonicalFilePath());
				}
				continue;
			}
			const QFileInfo info(candidate);
			if (info.exists() && info.isFile())
			{
				resolved.append(info.canonicalFilePath());
			}
		}
		resolved.removeDuplicates();
		return resolved;
	}

	QString RuntimeScanner::executableName()
	{
#if defined(Q_OS_WIN32)
		return QStringLiteral("javaw.exe");
#else
		return QStringLiteral("java");
#endif
	}

	QStringList RuntimeScanner::appendEnvPaths(const QStringList& base) const
	{
		QStringList expanded = base;
		auto env			 = qEnvironmentVariable("PROJTLAUNCHER_JAVA_PATHS");
#if defined(Q_OS_WIN32)
		QStringList javaPaths = env.replace("\\", "/").split(QLatin1String(";"), Qt::SkipEmptyParts);

		auto envPath			= qEnvironmentVariable("PATH");
		QStringList pathEntries = envPath.replace("\\", "/").split(QLatin1String(";"), Qt::SkipEmptyParts);
		for (const QString& entry : pathEntries)
		{
			javaPaths.append(entry + "/" + executableName());
		}
#else
		QStringList javaPaths = env.split(QLatin1String(":"), Qt::SkipEmptyParts);
#endif
		for (const QString& entry : javaPaths)
		{
			expanded.append(entry);
		}
		return expanded;
	}

#if defined(Q_OS_WIN32)
	QStringList RuntimeScanner::collectRegistryPaths(DWORD keyType,
													 const QString& keyName,
													 const QString& valueName,
													 const QString& suffix) const
	{
		QStringList entries;

		for (HKEY baseRegistry : { HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE })
		{
			HKEY rootKey;
			if (RegOpenKeyExW(baseRegistry,
							  keyName.toStdWString().c_str(),
							  0,
							  KEY_READ | keyType | KEY_ENUMERATE_SUB_KEYS,
							  &rootKey)
				!= ERROR_SUCCESS)
			{
				continue;
			}

			DWORD subKeyCount = 0;
			RegQueryInfoKeyW(rootKey, NULL, NULL, NULL, &subKeyCount, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
			if (subKeyCount > 0)
			{
				for (DWORD i = 0; i < subKeyCount; i++)
				{
					WCHAR subKeyName[255];
					DWORD subKeyNameSize = 255;
					auto retCode = RegEnumKeyExW(rootKey, i, subKeyName, &subKeyNameSize, NULL, NULL, NULL, NULL);
					if (retCode != ERROR_SUCCESS)
					{
						continue;
					}

					QString versionKey = keyName + "\\" + QString::fromWCharArray(subKeyName) + suffix;
					HKEY versionHandle;
					if (RegOpenKeyExW(baseRegistry,
									  versionKey.toStdWString().c_str(),
									  0,
									  KEY_READ | keyType,
									  &versionHandle)
						!= ERROR_SUCCESS)
					{
						continue;
					}

					DWORD valueSize = 0;
					if (RegQueryValueExW(versionHandle, valueName.toStdWString().c_str(), NULL, NULL, NULL, &valueSize)
						== ERROR_SUCCESS)
					{
						std::vector<WCHAR> buffer(valueSize / sizeof(WCHAR) + 1, 0);
						RegQueryValueExW(versionHandle,
										 valueName.toStdWString().c_str(),
										 NULL,
										 NULL,
										 reinterpret_cast<BYTE*>(buffer.data()),
										 &valueSize);
						QString javaHome = QString::fromWCharArray(buffer.data());
						QString javaPath = QDir(FS::PathCombine(javaHome, "bin")).absoluteFilePath(executableName());
						entries.append(javaPath);
					}
					RegCloseKey(versionHandle);
				}
			}

			RegCloseKey(rootKey);
		}
		return entries;
	}
#endif

	QStringList RuntimeScanner::collectManagedBundles()
	{
		QStringList bundles;

		auto addBundlePaths = [&bundles](const QString& prefix)
		{
			bundles.append(FS::PathCombine(prefix, "jre", "bin", RuntimeScanner::executableName()));
			bundles.append(FS::PathCombine(prefix, "bin", RuntimeScanner::executableName()));
			bundles.append(FS::PathCombine(prefix, RuntimeScanner::executableName()));
		};

		auto scanJavaDir = [&addBundlePaths](const QString& dirPath)
		{
			QDir dir(dirPath);
			if (!dir.exists())
			{
				return;
			}
			auto entries = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
			for (const auto& entry : entries)
			{
				addBundlePaths(entry.canonicalFilePath());
			}
		};

		scanJavaDir(APPLICATION->javaPath());

		return bundles;
	}

	QStringList RuntimeScanner::collectMinecraftBundles()
	{
		QStringList processPaths;
#if defined(Q_OS_MACOS)
		processPaths << FS::PathCombine(QDir::homePath(),
										FS::PathCombine("Library", "Application Support", "minecraft", "runtime"));
#elif defined(Q_OS_WIN32)
		auto appDataPath = QProcessEnvironment::systemEnvironment().value("APPDATA", "");
		processPaths << FS::PathCombine(QFileInfo(appDataPath).absoluteFilePath(), ".minecraft", "runtime");

		auto localAppDataPath	= QProcessEnvironment::systemEnvironment().value("LOCALAPPDATA", "");
		auto minecraftStorePath = FS::PathCombine(QFileInfo(localAppDataPath).absoluteFilePath(),
												  "Packages",
												  "Microsoft.4297127D64EC6_8wekyb3d8bbwe");
		processPaths << FS::PathCombine(minecraftStorePath, "LocalCache", "Local", "runtime");
#else
		processPaths << FS::PathCombine(QDir::homePath(), ".minecraft", "runtime");
#endif

		QStringList results;
		while (!processPaths.isEmpty())
		{
			auto dirPath = processPaths.takeFirst();
			QDir dir(dirPath);
			if (!dir.exists())
			{
				continue;
			}
			auto entries  = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
			bool foundBin = false;
			for (const auto& entry : entries)
			{
				if (entry.baseName() == "bin")
				{
					results.append(FS::PathCombine(entry.canonicalFilePath(), RuntimeScanner::executableName()));
					foundBin = true;
					break;
				}
			}
			if (!foundBin)
			{
				for (const auto& entry : entries)
				{
					processPaths << entry.canonicalFilePath();
				}
			}
		}
		return results;
	}

	QStringList RuntimeScanner::collectPaths(bool managedOnly) const
	{
		QStringList candidates;
		if (managedOnly)
		{
			candidates = collectManagedBundles();
			return resolveExecutables(candidates);
		}

#if defined(Q_OS_WIN32)
		candidates.append(
			collectRegistryPaths(KEY_WOW64_64KEY, "SOFTWARE\\JavaSoft\\Java Runtime Environment", "JavaHome", ""));
		candidates.append(
			collectRegistryPaths(KEY_WOW64_64KEY, "SOFTWARE\\JavaSoft\\Java Development Kit", "JavaHome", ""));
		candidates.append(
			collectRegistryPaths(KEY_WOW64_32KEY, "SOFTWARE\\JavaSoft\\Java Runtime Environment", "JavaHome", ""));
		candidates.append(
			collectRegistryPaths(KEY_WOW64_32KEY, "SOFTWARE\\JavaSoft\\Java Development Kit", "JavaHome", ""));

		candidates.append(collectRegistryPaths(KEY_WOW64_64KEY, "SOFTWARE\\JavaSoft\\JRE", "JavaHome", ""));
		candidates.append(collectRegistryPaths(KEY_WOW64_64KEY, "SOFTWARE\\JavaSoft\\JDK", "JavaHome", ""));
		candidates.append(collectRegistryPaths(KEY_WOW64_32KEY, "SOFTWARE\\JavaSoft\\JRE", "JavaHome", ""));
		candidates.append(collectRegistryPaths(KEY_WOW64_32KEY, "SOFTWARE\\JavaSoft\\JDK", "JavaHome", ""));

		candidates.append(
			collectRegistryPaths(KEY_WOW64_32KEY, "SOFTWARE\\AdoptOpenJDK\\JRE", "Path", "\\hotspot\\MSI"));
		candidates.append(
			collectRegistryPaths(KEY_WOW64_64KEY, "SOFTWARE\\AdoptOpenJDK\\JRE", "Path", "\\hotspot\\MSI"));
		candidates.append(
			collectRegistryPaths(KEY_WOW64_32KEY, "SOFTWARE\\AdoptOpenJDK\\JDK", "Path", "\\hotspot\\MSI"));
		candidates.append(
			collectRegistryPaths(KEY_WOW64_64KEY, "SOFTWARE\\AdoptOpenJDK\\JDK", "Path", "\\hotspot\\MSI"));

		candidates.append(
			collectRegistryPaths(KEY_WOW64_32KEY, "SOFTWARE\\Eclipse Foundation\\JDK", "Path", "\\hotspot\\MSI"));
		candidates.append(
			collectRegistryPaths(KEY_WOW64_64KEY, "SOFTWARE\\Eclipse Foundation\\JDK", "Path", "\\hotspot\\MSI"));

		candidates.append(
			collectRegistryPaths(KEY_WOW64_32KEY, "SOFTWARE\\Eclipse Adoptium\\JRE", "Path", "\\hotspot\\MSI"));
		candidates.append(
			collectRegistryPaths(KEY_WOW64_64KEY, "SOFTWARE\\Eclipse Adoptium\\JRE", "Path", "\\hotspot\\MSI"));
		candidates.append(
			collectRegistryPaths(KEY_WOW64_32KEY, "SOFTWARE\\Eclipse Adoptium\\JDK", "Path", "\\hotspot\\MSI"));
		candidates.append(
			collectRegistryPaths(KEY_WOW64_64KEY, "SOFTWARE\\Eclipse Adoptium\\JDK", "Path", "\\hotspot\\MSI"));

		candidates.append(collectRegistryPaths(KEY_WOW64_32KEY, "SOFTWARE\\Semeru\\JRE", "Path", "\\openj9\\MSI"));
		candidates.append(collectRegistryPaths(KEY_WOW64_64KEY, "SOFTWARE\\Semeru\\JRE", "Path", "\\openj9\\MSI"));
		candidates.append(collectRegistryPaths(KEY_WOW64_32KEY, "SOFTWARE\\Semeru\\JDK", "Path", "\\openj9\\MSI"));
		candidates.append(collectRegistryPaths(KEY_WOW64_64KEY, "SOFTWARE\\Semeru\\JDK", "Path", "\\openj9\\MSI"));

		candidates.append(collectRegistryPaths(KEY_WOW64_64KEY, "SOFTWARE\\Microsoft\\JDK", "Path", "\\hotspot\\MSI"));

		candidates.append(
			collectRegistryPaths(KEY_WOW64_64KEY, "SOFTWARE\\Azul Systems\\Zulu", "InstallationPath", ""));
		candidates.append(
			collectRegistryPaths(KEY_WOW64_32KEY, "SOFTWARE\\Azul Systems\\Zulu", "InstallationPath", ""));

		candidates.append(
			collectRegistryPaths(KEY_WOW64_64KEY, "SOFTWARE\\BellSoft\\Liberica", "InstallationPath", ""));
		candidates.append(
			collectRegistryPaths(KEY_WOW64_32KEY, "SOFTWARE\\BellSoft\\Liberica", "InstallationPath", ""));

		candidates.append("C:/Program Files/Java/jre8/bin/javaw.exe");
		candidates.append("C:/Program Files/Java/jre7/bin/javaw.exe");
		candidates.append("C:/Program Files/Java/jre6/bin/javaw.exe");
		candidates.append("C:/Program Files (x86)/Java/jre8/bin/javaw.exe");
		candidates.append("C:/Program Files (x86)/Java/jre7/bin/javaw.exe");
		candidates.append("C:/Program Files (x86)/Java/jre6/bin/javaw.exe");

		candidates.append("javaw");
#elif defined(Q_OS_MAC)
		candidates.append("java");
		candidates.append(
			"/Applications/Xcode.app/Contents/Applications/Application Loader.app/Contents/MacOS/itms/java/bin/java");
		candidates.append("/Library/Internet Plug-Ins/JavaAppletPlugin.plugin/Contents/Home/bin/java");
		candidates.append("/System/Library/Frameworks/JavaVM.framework/Versions/Current/Commands/java");

		QDir libraryJVMDir("/Library/Java/JavaVirtualMachines/");
		QStringList libraryJVMJavas = libraryJVMDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
		for (const QString& java : libraryJVMJavas)
		{
			candidates.append(libraryJVMDir.absolutePath() + "/" + java + "/Contents/Home/bin/java");
			candidates.append(libraryJVMDir.absolutePath() + "/" + java + "/Contents/Home/jre/bin/java");
		}

		QDir systemLibraryJVMDir("/System/Library/Java/JavaVirtualMachines/");
		QStringList systemLibraryJVMJavas = systemLibraryJVMDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
		for (const QString& java : systemLibraryJVMJavas)
		{
			candidates.append(systemLibraryJVMDir.absolutePath() + "/" + java + "/Contents/Home/bin/java");
			candidates.append(systemLibraryJVMDir.absolutePath() + "/" + java + "/Contents/Commands/java");
		}

		auto home = qEnvironmentVariable("HOME");

		QString sdkmanDir = qEnvironmentVariable("SDKMAN_DIR", FS::PathCombine(home, ".sdkman"));
		QDir sdkmanJavaDir(FS::PathCombine(sdkmanDir, "candidates/java"));
		QStringList sdkmanJavas = sdkmanJavaDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
		for (const QString& java : sdkmanJavas)
		{
			candidates.append(sdkmanJavaDir.absolutePath() + "/" + java + "/bin/java");
		}

		QString asdfDataDir = qEnvironmentVariable("ASDF_DATA_DIR", FS::PathCombine(home, ".asdf"));
		QDir asdfJavaDir(FS::PathCombine(asdfDataDir, "installs/java"));
		QStringList asdfJavas = asdfJavaDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
		for (const QString& java : asdfJavas)
		{
			candidates.append(asdfJavaDir.absolutePath() + "/" + java + "/bin/java");
		}

		QDir userLibraryJVMDir(FS::PathCombine(home, "Library/Java/JavaVirtualMachines/"));
		QStringList userLibraryJVMJavas = userLibraryJVMDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
		for (const QString& java : userLibraryJVMJavas)
		{
			candidates.append(userLibraryJVMDir.absolutePath() + "/" + java + "/Contents/Home/bin/java");
			candidates.append(userLibraryJVMDir.absolutePath() + "/" + java + "/Contents/Commands/java");
		}
#elif defined(Q_OS_LINUX) || defined(Q_OS_OPENBSD) || defined(Q_OS_FREEBSD)
		candidates.append("java");
		auto scanJavaDir =
			[&candidates](
				const QString& dirPath,
				const std::function<bool(const QFileInfo&)>& filter = [](const QFileInfo&) { return true; })
		{
			QDir dir(dirPath);
			if (!dir.exists())
				return;
			auto entries = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
			for (const auto& entry : entries)
			{
				if (!filter(entry))
					continue;
				QString prefix = entry.canonicalFilePath();
				candidates.append(FS::PathCombine(prefix, "jre/bin/java"));
				candidates.append(FS::PathCombine(prefix, "bin/java"));
			}
		};
		auto snap		  = qEnvironmentVariable("SNAP");
		auto scanJavaDirs = [scanJavaDir, snap](const QString& dirPath)
		{
			scanJavaDir(dirPath);
			if (!snap.isNull())
			{
				scanJavaDir(snap + dirPath);
			}
		};
#if defined(Q_OS_LINUX)
		scanJavaDirs("/usr/java");
		scanJavaDirs("/usr/lib/jvm");
		scanJavaDirs("/usr/lib64/jvm");
		scanJavaDirs("/usr/lib32/jvm");
		auto gentooFilter = [](const QFileInfo& info)
		{
			QString fileName = info.fileName();
			return fileName.startsWith("openjdk-") || fileName.startsWith("openj9-");
		};
		auto aoscFilter = [](const QFileInfo& info)
		{
			QString fileName = info.fileName();
			return fileName == "java" || fileName.startsWith("java-");
		};
		scanJavaDir("/usr/lib64", gentooFilter);
		scanJavaDir("/usr/lib", gentooFilter);
		scanJavaDir("/opt", gentooFilter);
		scanJavaDir("/usr/lib", aoscFilter);
		scanJavaDirs("java");
		scanJavaDirs("/opt/jdk");
		scanJavaDirs("/opt/jdks");
		scanJavaDirs("/opt/ibm");
		scanJavaDirs("/app/jdk");
#elif defined(Q_OS_OPENBSD) || defined(Q_OS_FREEBSD)
		scanJavaDirs("/usr/local");
#endif
		auto home = qEnvironmentVariable("HOME");
		scanJavaDirs(FS::PathCombine(home, ".jdks"));
		QString sdkmanDir = qEnvironmentVariable("SDKMAN_DIR", FS::PathCombine(home, ".sdkman"));
		scanJavaDirs(FS::PathCombine(sdkmanDir, "candidates/java"));
		QString asdfDataDir = qEnvironmentVariable("ASDF_DATA_DIR", FS::PathCombine(home, ".asdf"));
		scanJavaDirs(FS::PathCombine(asdfDataDir, "installs/java"));
		scanJavaDirs(FS::PathCombine(home, ".gradle/jdks"));
#else
		candidates.append("java");
#endif

		candidates.append(collectMinecraftBundles());
		candidates.append(collectManagedBundles());
		candidates = appendEnvPaths(candidates);
		return resolveExecutables(candidates);
	}
} // namespace projt::java
