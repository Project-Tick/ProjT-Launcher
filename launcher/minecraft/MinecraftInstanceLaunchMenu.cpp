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

#include "MinecraftInstanceLaunchMenu.h"

#include <QAction>
#include <QActionGroup>
#include <QKeySequence>

#include <Application.h>
#include "tools/BaseProfiler.h"

#include "MinecraftInstance.h"

void MinecraftInstanceLaunchMenu::populate(MinecraftInstance* instance, QMenu* menu)
{
	QAction* normalLaunch = menu->addAction(MinecraftInstance::tr("&Launch"));
	normalLaunch->setShortcut(QKeySequence::Open);
	QAction* normalLaunchOffline = menu->addAction(MinecraftInstance::tr("Launch &Offline"));
	normalLaunchOffline->setShortcut(QKeySequence(MinecraftInstance::tr("Ctrl+Shift+O")));
	QAction* normalLaunchDemo = menu->addAction(MinecraftInstance::tr("Launch &Demo"));
	normalLaunchDemo->setShortcut(QKeySequence(MinecraftInstance::tr("Ctrl+Alt+O")));

	normalLaunchDemo->setEnabled(instance->supportsDemo());

	QObject::connect(normalLaunch,
					 &QAction::triggered,
					 [instance] { APPLICATION->launch(instance->shared_from_this()); });
	QObject::connect(normalLaunchOffline,
					 &QAction::triggered,
					 [instance] { APPLICATION->launch(instance->shared_from_this(), LaunchMode::Offline); });
	QObject::connect(normalLaunchDemo,
					 &QAction::triggered,
					 [instance] { APPLICATION->launch(instance->shared_from_this(), LaunchMode::Demo); });

	QString profilersTitle = MinecraftInstance::tr("Profilers");
	menu->addSeparator()->setText(profilersTitle);

	auto profilers = new QActionGroup(menu);
	profilers->setExclusive(true);
	QObject::connect(profilers,
					 &QActionGroup::triggered,
					 [instance](QAction* action)
					 {
						 instance->settings()->set("Profiler", action->data());
						 emit instance->profilerChanged();
					 });

	QAction* noProfilerAction = menu->addAction(MinecraftInstance::tr("&No Profiler"));
	noProfilerAction->setData("");
	noProfilerAction->setCheckable(true);
	noProfilerAction->setChecked(true);
	profilers->addAction(noProfilerAction);

	for (auto profiler = APPLICATION->profilers().begin(); profiler != APPLICATION->profilers().end(); profiler++)
	{
		QAction* profilerAction = menu->addAction(profiler.value()->name());
		profilers->addAction(profilerAction);
		profilerAction->setData(profiler.key());
		profilerAction->setCheckable(true);
		profilerAction->setChecked(instance->settings()->get("Profiler").toString() == profiler.key());

		QString error;
		profilerAction->setEnabled(profiler.value()->check(&error));
	}
}
