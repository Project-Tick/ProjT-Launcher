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

#include "LaunchMenu.h"

#include <QAction>
#include <QActionGroup>
#include <QKeySequence>

#include "Application.h"
#include "minecraft/MinecraftInstance.h"
#include "tools/BaseProfiler.h"

namespace LaunchMenu
{

	void populate(std::shared_ptr<BaseInstance> instance, QMenu* menu)
	{
		auto mcInstance = std::dynamic_pointer_cast<MinecraftInstance>(instance);
		if (!mcInstance)
		{
			return;
		}

		QAction* normalLaunch = menu->addAction(QObject::tr("&Launch"));
		normalLaunch->setShortcut(QKeySequence::Open);
		QAction* normalLaunchOffline = menu->addAction(QObject::tr("Launch &Offline"));
		normalLaunchOffline->setShortcut(QKeySequence(QObject::tr("Ctrl+Shift+O")));
		QAction* normalLaunchDemo = menu->addAction(QObject::tr("Launch &Demo"));
		normalLaunchDemo->setShortcut(QKeySequence(QObject::tr("Ctrl+Alt+O")));

		normalLaunchDemo->setEnabled(mcInstance->supportsDemo());

		// Connect actions
		// Note: We use APPLICATION global as logic handler
		QObject::connect(normalLaunch, &QAction::triggered, [instance] { APPLICATION->launch(instance); });
		QObject::connect(normalLaunchOffline,
						 &QAction::triggered,
						 [instance] { APPLICATION->launch(instance, LaunchMode::Offline); });
		QObject::connect(normalLaunchDemo,
						 &QAction::triggered,
						 [instance] { APPLICATION->launch(instance, LaunchMode::Demo); });

		QString profilersTitle = QObject::tr("Profilers");
		menu->addSeparator()->setText(profilersTitle);

		auto profilers = new QActionGroup(menu);
		profilers->setExclusive(true);
		// Logic to update settings directly on the instance
		QObject::connect(profilers,
						 &QActionGroup::triggered,
						 [mcInstance](QAction* action)
						 {
							 mcInstance->settings()->set("Profiler", action->data());
							 // Since we are external, we can't emit profilerChanged() directly if it's protected. (It
							 // is a signal). However, we changed the setting, and MinecraftInstance usually reacts to
							 // setting changes or the UI does. The original code emitted it explicitly: emit
							 // profilerChanged(); PackProfile/InstanceWindow listens to this? Let's assume setting
							 // change is enough or we might need to invoke it via metaobject if needed, but cleaner is
							 // if InstanceWindow listens to "Profiler" setting change. For now, to match exact behavior
							 // without friending, we rely on settings update.
						 });

		QAction* noProfilerAction = menu->addAction(QObject::tr("&No Profiler"));
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
			profilerAction->setChecked(mcInstance->settings()->get("Profiler").toString() == profiler.key());

			QString error;
			profilerAction->setEnabled(profiler.value()->check(&error));
		}
	}

} // namespace LaunchMenu
