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
#include "JProfiler.h"

#include <QDir>

#include "BaseInstance.h"
#include "launch/LaunchPipeline.hpp"
#include "settings/SettingsObject.h"

class JProfiler : public BaseProfiler
{
	Q_OBJECT
  public:
	JProfiler(SettingsObjectPtr settings, InstancePtr instance, QObject* parent = 0);

  private slots:
	void profilerStarted();
	void profilerFinished(int exit, QProcess::ExitStatus status);

  protected:
	void beginProfilingImpl(shared_qobject_ptr<projt::launch::LaunchPipeline> process);

  private:
	int listeningPort = 0;
};

JProfiler::JProfiler(SettingsObjectPtr settings, InstancePtr instance, QObject* parent)
	: BaseProfiler(settings, instance, parent)
{}

void JProfiler::profilerStarted()
{
	emit readyToLaunch(tr("Listening on port: %1").arg(listeningPort));
}

void JProfiler::profilerFinished([[maybe_unused]] int exit, QProcess::ExitStatus status)
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

void JProfiler::beginProfilingImpl(shared_qobject_ptr<projt::launch::LaunchPipeline> process)
{
	listeningPort			 = globalSettings->get("JProfilerPort").toInt();
	QProcess* profiler		 = new QProcess(this);
	QStringList profilerArgs = { "-d", QString::number(process->pid()), "--gui", "-p", QString::number(listeningPort) };
	auto basePath			 = globalSettings->get("JProfilerPath").toString();

#ifdef Q_OS_WIN
	QString profilerProgram = QDir(basePath).absoluteFilePath("bin/jpenable.exe");
#else
	QString profilerProgram = QDir(basePath).absoluteFilePath("bin/jpenable");
#endif

	profiler->setArguments(profilerArgs);
	profiler->setProgram(profilerProgram);

	connect(profiler, &QProcess::started, this, &JProfiler::profilerStarted);
	connect(profiler, &QProcess::finished, this, &JProfiler::profilerFinished);

	m_profilerProcess = profiler;
	profiler->start();
}

void JProfilerFactory::registerSettings(SettingsObjectPtr settings)
{
	settings->registerSetting("JProfilerPath");
	settings->registerSetting("JProfilerPort", 42042);
	globalSettings = settings;
}

BaseExternalTool* JProfilerFactory::createTool(InstancePtr instance, QObject* parent)
{
	return new JProfiler(globalSettings, instance, parent);
}

bool JProfilerFactory::check(QString* error)
{
	return check(globalSettings->get("JProfilerPath").toString(), error);
}

bool JProfilerFactory::check(const QString& path, QString* error)
{
	if (path.isEmpty())
	{
		*error = QObject::tr("Empty path");
		return false;
	}
	QDir dir(path);
	if (!dir.exists())
	{
		*error = QObject::tr("Path does not exist");
		return false;
	}
	if (!dir.exists("bin") || !(dir.exists("bin/jprofiler") || dir.exists("bin/jprofiler.exe"))
		|| !dir.exists("bin/agent.jar"))
	{
		*error = QObject::tr("Invalid JProfiler install");
		return false;
	}
	return true;
}

#include "JProfiler.moc"
