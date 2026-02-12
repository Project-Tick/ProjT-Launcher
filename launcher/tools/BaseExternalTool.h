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

#include <BaseInstance.h>
#include <QObject>

class BaseInstance;
class SettingsObject;
class QProcess;

class BaseExternalTool : public QObject
{
	Q_OBJECT
  public:
	explicit BaseExternalTool(SettingsObjectPtr settings, InstancePtr instance, QObject* parent = 0);
	virtual ~BaseExternalTool();

  protected:
	InstancePtr m_instance;
	SettingsObjectPtr globalSettings;
};

class BaseDetachedTool : public BaseExternalTool
{
	Q_OBJECT
  public:
	explicit BaseDetachedTool(SettingsObjectPtr settings, InstancePtr instance, QObject* parent = 0);

  public slots:
	void run();

  protected:
	virtual void runImpl() = 0;
};

class BaseExternalToolFactory
{
  public:
	virtual ~BaseExternalToolFactory();

	virtual QString name() const = 0;

	virtual void registerSettings(SettingsObjectPtr settings) = 0;

	virtual BaseExternalTool* createTool(InstancePtr instance, QObject* parent = 0) = 0;

	virtual bool check(QString* error)						= 0;
	virtual bool check(const QString& path, QString* error) = 0;

  protected:
	SettingsObjectPtr globalSettings;
};

class BaseDetachedToolFactory : public BaseExternalToolFactory
{
  public:
	virtual BaseDetachedTool* createDetachedTool(InstancePtr instance, QObject* parent = 0);
};
