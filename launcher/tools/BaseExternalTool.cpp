// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
 *
 *  This file is part of ProjT Launcher and is licensed under
 *  the GNU General Public License version 3 or later.
 *
 *  If this file includes work from previous open-source projects,
 *  their original copyright and license notices are preserved below.
 */
#include "BaseExternalTool.h"

#include <QDir>
#include <QProcess>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#include "BaseInstance.h"

BaseExternalTool::BaseExternalTool(SettingsObjectPtr settings, InstancePtr instance, QObject* parent)
    : QObject(parent), m_instance(instance), globalSettings(settings)
{}

BaseExternalTool::~BaseExternalTool() {}

BaseDetachedTool::BaseDetachedTool(SettingsObjectPtr settings, InstancePtr instance, QObject* parent)
    : BaseExternalTool(settings, instance, parent)
{}

void BaseDetachedTool::run()
{
    runImpl();
}

BaseExternalToolFactory::~BaseExternalToolFactory() {}

BaseDetachedTool* BaseDetachedToolFactory::createDetachedTool(InstancePtr instance, QObject* parent)
{
    return qobject_cast<BaseDetachedTool*>(createTool(instance, parent));
}
