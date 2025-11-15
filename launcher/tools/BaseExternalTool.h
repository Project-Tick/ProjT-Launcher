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

#include <BaseInstance.h>
#include <QObject>

class BaseInstance;
class SettingsObject;
class QProcess;

class BaseExternalTool : public QObject {
    Q_OBJECT
   public:
    explicit BaseExternalTool(SettingsObjectPtr settings, InstancePtr instance, QObject* parent = 0);
    virtual ~BaseExternalTool();

   protected:
    InstancePtr m_instance;
    SettingsObjectPtr globalSettings;
};

class BaseDetachedTool : public BaseExternalTool {
    Q_OBJECT
   public:
    explicit BaseDetachedTool(SettingsObjectPtr settings, InstancePtr instance, QObject* parent = 0);

   public slots:
    void run();

   protected:
    virtual void runImpl() = 0;
};

class BaseExternalToolFactory {
   public:
    virtual ~BaseExternalToolFactory();

    virtual QString name() const = 0;

    virtual void registerSettings(SettingsObjectPtr settings) = 0;

    virtual BaseExternalTool* createTool(InstancePtr instance, QObject* parent = 0) = 0;

    virtual bool check(QString* error) = 0;
    virtual bool check(const QString& path, QString* error) = 0;

   protected:
    SettingsObjectPtr globalSettings;
};

class BaseDetachedToolFactory : public BaseExternalToolFactory {
   public:
    virtual BaseDetachedTool* createDetachedTool(InstancePtr instance, QObject* parent = 0);
};
