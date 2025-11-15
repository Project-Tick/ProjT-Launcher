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
#pragma once

#include "minecraft/Component.h"
#include "net/Mode.h"
#include "tasks/Task.h"

#include <memory>
class PackProfile;
struct ComponentUpdateTaskData;

class ComponentUpdateTask : public Task {
    Q_OBJECT
   public:
    enum class Mode { Launch, Resolution };

   public:
    explicit ComponentUpdateTask(Mode mode, Net::Mode netmode, PackProfile* list);
    virtual ~ComponentUpdateTask();

   protected:
    void executeTask();

   private:
    void loadComponents();
    /// collects components that are dependent on or dependencies of the component
    QList<ComponentPtr> collectTreeLinked(const QString& uid);
    void resolveDependencies(bool checkOnly);
    void performUpdateActions();
    void finalizeComponents();

    void remoteLoadSucceeded(size_t index);
    void remoteLoadFailed(size_t index, const QString& msg);
    void checkIfAllFinished();

   private:
    std::unique_ptr<ComponentUpdateTaskData> d;
};
