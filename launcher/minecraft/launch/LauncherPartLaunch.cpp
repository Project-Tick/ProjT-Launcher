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

#include "LauncherPartLaunch.hpp"

#include <QRegularExpression>
#include <QStandardPaths>
#include <QStringConverter>

#include "Application.h"
#include "Commandline.h"
#include "FileSystem.h"
#include "launch/LaunchPipeline.hpp"
#include "minecraft/MinecraftInstance.h"

#ifdef Q_OS_LINUX
#include "gamemode_client.h"
#endif

LauncherPartLaunch::LauncherPartLaunch(projt::launch::LaunchPipeline* parent)
	: projt::launch::LaunchStage(parent),
	  m_process(parent->instance()->getRuntimeVersion().defaultsToUtf8() ? QStringConverter::Utf8
																		 : QStringConverter::System)
{
	if (parent->instance()->settings()->get("CloseAfterLaunch").toBool())
	{
		static const QRegularExpression s_settingUser(".*Setting user.+", QRegularExpression::CaseInsensitiveOption);
		std::shared_ptr<QMetaObject::Connection> connection{ new QMetaObject::Connection };
		*connection = connect(&m_process,
							  &LoggedProcess::log,
							  this,
							  [connection](const QStringList& lines, [[maybe_unused]] MessageLevel::Enum level)
							  {
								  qDebug() << lines;
								  if (lines.filter(s_settingUser).length() != 0)
								  {
									  APPLICATION->closeAllWindows();
									  disconnect(*connection);
								  }
							  });
	}

	connect(&m_process, &LoggedProcess::log, this, &LauncherPartLaunch::logLines);
	connect(&m_process, &LoggedProcess::stateChanged, this, &LauncherPartLaunch::on_state);
}

void LauncherPartLaunch::executeTask()
{
	QString jarPath = APPLICATION->getJarPath("ProjTLaunch.jar");
	if (jarPath.isEmpty())
	{
		const char* reason = QT_TR_NOOP("Launcher library could not be found. Please check your installation.");
		emit logLine(tr(reason), MessageLevel::Fatal);
		emitFailed(tr(reason));
		return;
	}

	auto instance = m_flow->instance();

	QString legacyJarPath;
	if (instance->getLauncher() == "legacy" || instance->shouldApplyOnlineFixes())
	{
		legacyJarPath = APPLICATION->getJarPath("ProjTLaunchLegacy.jar");
		if (legacyJarPath.isEmpty())
		{
			const char* reason =
				QT_TR_NOOP("Legacy launcher library could not be found. Please check your installation.");
			emit logLine(tr(reason), MessageLevel::Fatal);
			emitFailed(tr(reason));
			return;
		}
	}

	m_launchScript	 = instance->createLaunchScript(m_session, m_targetToJoin);
	QStringList args = instance->javaArguments();
	QString allArgs	 = args.join(", ");
	emit logLine("Java Arguments:\n[" + m_flow->censorPrivateInfo(allArgs) + "]\n\n", MessageLevel::Launcher);

	auto javaPath = FS::ResolveExecutable(instance->settings()->get("JavaPath").toString());

	m_process.setProcessEnvironment(instance->createLaunchEnvironment());

	// make detachable - this will keep the process running even if the object is destroyed
	m_process.setDetachable(true);

	auto classPath = instance->getClassPath();
	classPath.prepend(jarPath);

	if (!legacyJarPath.isEmpty())
		classPath.prepend(legacyJarPath);

	auto natPath = instance->getNativePath();
#ifdef Q_OS_WIN
	natPath = FS::getPathNameInLocal8bit(natPath);
#endif
	args << "-Djava.library.path=" + natPath;

	args << "-cp";
#ifdef Q_OS_WIN
	QStringList processed;
	for (auto& item : classPath)
	{
		processed << FS::getPathNameInLocal8bit(item);
	}
	args << processed.join(';');
#else
	args << classPath.join(':');
#endif
	args << "org.projecttick.projtlauncher.normal.EntryPoint";

	qDebug() << args.join(' ');

	QString wrapperCommandStr = instance->getWrapperCommand().trimmed();
	if (!wrapperCommandStr.isEmpty())
	{
		wrapperCommandStr		= m_flow->substituteVariables(wrapperCommandStr);
		auto wrapperArgs		= Commandline::splitArgs(wrapperCommandStr);
		auto wrapperCommand		= wrapperArgs.takeFirst();
		auto realWrapperCommand = QStandardPaths::findExecutable(wrapperCommand);
		if (realWrapperCommand.isEmpty())
		{
			const char* reason = QT_TR_NOOP("The wrapper command \"%1\" couldn't be found.");
			emit logLine(QString(reason).arg(wrapperCommand), MessageLevel::Fatal);
			emitFailed(tr(reason).arg(wrapperCommand));
			return;
		}
		emit logLine("Wrapper command is:\n" + wrapperCommandStr + "\n\n", MessageLevel::Launcher);
		args.prepend(javaPath);
		m_process.start(wrapperCommand, wrapperArgs + args);
	}
	else
	{
		m_process.start(javaPath, args);
	}

#ifdef Q_OS_LINUX
	if (instance->settings()->get("EnableFeralGamemode").toBool()
		&& APPLICATION->capabilities() & Application::SupportsGameMode)
	{
		auto pid = m_process.processId();
		if (pid)
		{
			gamemode_request_start_for(pid);
		}
	}
#endif
}

void LauncherPartLaunch::on_state(LoggedProcess::State state)
{
	switch (state)
	{
		case LoggedProcess::FailedToStart:
		{
			//: Error message displayed if instace can't start
			const char* reason = QT_TR_NOOP("Could not launch Minecraft!");
			emit logLine(reason, MessageLevel::Fatal);
			emitFailed(tr(reason));
			return;
		}
		case LoggedProcess::Aborted:
		case LoggedProcess::Crashed:
		{
			m_flow->setPid(-1);
			m_flow->instance()->setMinecraftRunning(false);
			emitFailed(tr("Game crashed."));
			return;
		}
		case LoggedProcess::Finished:
		{
			auto instance = m_flow->instance();
			if (instance->settings()->get("CloseAfterLaunch").toBool())
				APPLICATION->showMainWindow();

			m_flow->setPid(-1);
			m_flow->instance()->setMinecraftRunning(false);
			// if the exit code wasn't 0, report this as a crash
			auto exitCode = m_process.exitCode();
			if (exitCode != 0)
			{
				emitFailed(tr("Game crashed."));
				return;
			}

			// Set exit code in environment for post-launch hooks
			auto env = m_process.processEnvironment();
			env.insert("INST_EXITCODE", QString::number(exitCode));
			env.insert("INST_NAME", instance->name());
			env.insert("INST_ID", instance->id());
			env.insert("INST_DIR", instance->instanceRoot());
			// Post-launch command can be retrieved from instance settings
			QString postExitCmd = instance->settings()->get("PostExitCommand").toString();
			if (!postExitCmd.isEmpty())
			{
				emit logLine(tr("Running post-exit command: %1").arg(postExitCmd), MessageLevel::Launcher);
				QProcess postProcess;
				postProcess.setProcessEnvironment(env);
				postProcess.setWorkingDirectory(instance->instanceRoot());
				postProcess.start(postExitCmd);
				postProcess.waitForFinished(30000); // 30 second timeout
				if (postProcess.exitCode() != 0)
				{
					emit logLine(tr("Post-exit command failed with code %1").arg(postProcess.exitCode()),
								 MessageLevel::Warning);
				}
			}

			emitSucceeded();
			break;
		}
		case LoggedProcess::Running:
			emit logLine(QString("Minecraft process ID: %1\n\n").arg(m_process.processId()), MessageLevel::Launcher);
			m_flow->setPid(m_process.processId());
			// send the launch script to the launcher part
			m_process.write(m_launchScript.toUtf8());

			mayProceed = true;
			emit readyForLaunch();
			break;
		default: break;
	}
}

void LauncherPartLaunch::setWorkingDirectory(const QString& wd)
{
	m_process.setWorkingDirectory(wd);
}

void LauncherPartLaunch::proceed()
{
	if (mayProceed)
	{
		m_flow->instance()->setMinecraftRunning(true);
		QString launchString("launch\n");
		m_process.write(launchString.toUtf8());
		mayProceed = false;
	}
}

bool LauncherPartLaunch::abort()
{
	if (mayProceed)
	{
		mayProceed = false;
		QString launchString("abort\n");
		m_process.write(launchString.toUtf8());
	}
	else
	{
		auto state = m_process.state();
		if (state == LoggedProcess::Running || state == LoggedProcess::Starting)
		{
			m_process.kill();
		}
	}
	return true;
}
