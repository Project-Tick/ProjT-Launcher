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
 *  Linking this library statically or dynamically with other modules is
 *  making a combined work based on this library. Thus, the terms and
 *  conditions of the GNU General Public License cover the whole
 *  combination.
 *
 *  As a special exception, the copyright holders of this library give
 *  you permission to link this library with independent modules to
 *  produce an executable, regardless of the license terms of these
 *  independent modules, and to copy and distribute the resulting
 *  executable under terms of your choice, provided that you also meet,
 *  for each linked independent module, the terms and conditions of the
 *  license of that module. An independent module is a module which is
 *  not derived from or based on this library. If you modify this
 *  library, you may extend this exception to your version of the
 *  library, but you are not obliged to do so. If you do not wish to do
 *  so, delete this exception statement from your version.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * ======================================================================== */
package org.projecttick.projtlauncher.normal;

import org.projecttick.projtlauncher.normal.utils.Parameters;

public final class SystemProperties {
    public static void apply(Parameters params) {
        String launcherBrand = params.getString("launcherBrand", null);
        String launcherVersion = params.getString("launcherVersion", null);
        String name = params.getString("instanceName", null);
        String iconId = params.getString("instanceIconKey", null);
        String iconPath = params.getString("instanceIconPath", null);
        String windowTitle = params.getString("windowTitle", null);
        String windowDimensions = params.getString("windowParams", null);

        if (launcherBrand != null)
            System.setProperty("minecraft.launcher.brand", launcherBrand);
        if (launcherVersion != null)
            System.setProperty("minecraft.launcher.version", launcherVersion);

        // set useful properties for mods
        if (name != null)
            System.setProperty("org.projecttick.projtlauncher.normal.instance.name", name);
        if (iconId != null)
            System.setProperty("org.projecttick.projtlauncher.normal.instance.icon.id", iconId);
        if (iconPath != null)
            System.setProperty("org.projecttick.projtlauncher.normal.instance.icon.path", iconPath);
        if (windowTitle != null)
            System.setProperty("org.projecttick.projtlauncher.normal.window.title", windowTitle);
        if (windowDimensions != null)
            System.setProperty("org.projecttick.projtlauncher.normal.window.dimensions", windowDimensions);

        // set multimc properties for compatibility
        if (name != null)
            System.setProperty("multimc.instance.title", name);
        if (iconId != null)
            System.setProperty("multimc.instance.icon", iconId);
    }
}
