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

#include "VerifyJavaInstall.hpp"
#include <memory>

#include "Application.h"
#include "MessageLevel.h"
#include "java/core/RuntimeVersion.hpp"
#include "minecraft/MinecraftInstance.h"
#include "minecraft/PackProfile.h"

void VerifyJavaInstall::executeTask()
{
	auto instance			 = m_flow->instance();
	auto packProfile		 = instance->getPackProfile();
	auto settings			 = instance->settings();
	auto storedVersion		 = settings->get("JavaVersion").toString();
	auto ignoreCompatibility = settings->get("IgnoreJavaCompatibility").toBool();
	auto javaArchitecture	 = settings->get("JavaArchitecture").toString();
	auto maxMemAlloc		 = settings->get("MaxMemAlloc").toInt();

	if (javaArchitecture == "32" && maxMemAlloc > 2048)
	{
		emit logLine(tr("Max memory allocation exceeds the supported value.\n"
						"The selected installation of Java is 32-bit and doesn't support more than 2048MiB of RAM.\n"
						"The instance may not start due to this."),
					 MessageLevel::Error);
	}

	auto compatibleMajors = packProfile->getProfile()->getCompatibleJavaMajors();

	projt::java::RuntimeVersion javaVersion(storedVersion);

	if (compatibleMajors.isEmpty() || compatibleMajors.contains(javaVersion.major()))
	{
		emitSucceeded();
		return;
	}

	if (ignoreCompatibility)
	{
		emit logLine(tr("Java major version is incompatible. Things might break."), MessageLevel::Warning);
		emitSucceeded();
		return;
	}

	emit logLine(tr("This instance is not compatible with Java version %1.\n"
					"Please switch to one of the following Java versions for this instance:")
					 .arg(javaVersion.major()),
				 MessageLevel::Error);
	for (auto major : compatibleMajors)
	{
		emit logLine(tr("Java version %1").arg(major), MessageLevel::Error);
	}
	emit logLine(tr("Go to instance Java settings to change your Java version or disable the Java compatibility check "
					"if you know what "
					"you're doing."),
				 MessageLevel::Error);

	emitFailed(QString("Incompatible Java major version"));
}
