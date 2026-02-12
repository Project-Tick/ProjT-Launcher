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
#include "JVisualVM.h"

#include <QDir>
#include <QStandardPaths>

#include "BaseInstance.h"
#include "launch/LaunchPipeline.hpp"
#include "settings/SettingsObject.h"

class JVisualVM : public BaseProfiler
{
	Q_OBJECT
  public:
	JVisualVM(SettingsObjectPtr settings, InstancePtr instance, QObject* parent = 0);

  private slots:
	void profilerStarted();
	void profilerFinished(int exit, QProcess::ExitStatus status);

  protected:
	void beginProfilingImpl(shared_qobject_ptr<projt::launch::LaunchPipeline> process);
};

JVisualVM::JVisualVM(SettingsObjectPtr settings, InstancePtr instance, QObject* parent)
	: BaseProfiler(settings, instance, parent)
{}

void JVisualVM::profilerStarted()
{
	emit readyToLaunch(tr("VisualVM started"));
}

void JVisualVM::profilerFinished([[maybe_unused]] int exit, QProcess::ExitStatus status)
{
	if (status == QProcess::CrashExit)
	{
		emit abortLaunch(tr("Profiler aborted"));
	}
	if (m_profilerProcess)
	{
		m_profilerProcess->deleteLater();
		m_profilerProcess = 0;
	}
}

void JVisualVM::beginProfilingImpl(shared_qobject_ptr<projt::launch::LaunchPipeline> process)
{
	QProcess* profiler		 = new QProcess(this);
	QStringList profilerArgs = { "--openpid", QString::number(process->pid()) };
	auto programPath		 = globalSettings->get("JVisualVMPath").toString();

	profiler->setArguments(profilerArgs);
	profiler->setProgram(programPath);

	connect(profiler, &QProcess::started, this, &JVisualVM::profilerStarted);
	connect(profiler, &QProcess::finished, this, &JVisualVM::profilerFinished);

	profiler->start();
	m_profilerProcess = profiler;
}

void JVisualVMFactory::registerSettings(SettingsObjectPtr settings)
{
	QString defaultValue = QStandardPaths::findExecutable("jvisualvm");
	if (defaultValue.isNull())
	{
		defaultValue = QStandardPaths::findExecutable("visualvm");
	}
	settings->registerSetting("JVisualVMPath", defaultValue);
	globalSettings = settings;
}

BaseExternalTool* JVisualVMFactory::createTool(InstancePtr instance, QObject* parent)
{
	return new JVisualVM(globalSettings, instance, parent);
}

bool JVisualVMFactory::check(QString* error)
{
	return check(globalSettings->get("JVisualVMPath").toString(), error);
}

bool JVisualVMFactory::check(const QString& path, QString* error)
{
	if (path.isEmpty())
	{
		*error = QObject::tr("Empty path");
		return false;
	}
	QFileInfo finfo(path);
	if (!finfo.isExecutable() || !finfo.fileName().contains("visualvm"))
	{
		*error = QObject::tr("Invalid path to VisualVM");
		return false;
	}
	return true;
}

#include "JVisualVM.moc"
