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
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "java/services/RuntimeProbeTask.hpp"

#include <QDebug>
#include <QFile>
#include <QMap>
#include <utility>

#include "Application.h"
#include "Commandline.h"
#include "FileSystem.h"
#include "java/services/RuntimeEnvironment.hpp"

namespace projt::java
{
	RuntimeProbeTask::RuntimeProbeTask(ProbeSettings settings)
		: Task(),
		  m_settings(std::move(settings))
	{}

	QString RuntimeProbeTask::probeJarPath()
	{
		return APPLICATION->getJarPath("JavaCheck.jar");
	}

	void RuntimeProbeTask::executeTask()
	{
		QString checkerJar = probeJarPath();

		if (checkerJar.isEmpty())
		{
			qDebug() << "Java checker library could not be found. Please check your installation.";
			ProbeReport report;
			report.path = m_settings.binaryPath;
			report.token = m_settings.token;
			report.status = ProbeReport::Status::Errored;
			emit probeFinished(report);
			emitSucceeded();
			return;
		}
#ifdef Q_OS_WIN
		checkerJar = FS::getPathNameInLocal8bit(checkerJar);
#endif

		QStringList args;
		process.reset(new QProcess());
		if (!m_settings.extraArgs.isEmpty())
		{
			args.append(Commandline::splitArgs(m_settings.extraArgs));
		}
		if (m_settings.minMem != 0)
		{
			args << QString("-Xms%1m").arg(m_settings.minMem);
		}
		if (m_settings.maxMem != 0)
		{
			args << QString("-Xmx%1m").arg(m_settings.maxMem);
		}
		if (m_settings.permGen != 64 && m_settings.permGen != 0)
		{
			args << QString("-XX:PermSize=%1m").arg(m_settings.permGen);
		}

		args.append({ "-jar", checkerJar, "os.arch", "java.version", "java.vendor" });
		process->setArguments(args);
		process->setProgram(m_settings.binaryPath);
		process->setProcessChannelMode(QProcess::SeparateChannels);
		process->setProcessEnvironment(buildCleanEnvironment());
		qDebug() << "Running runtime probe:" << m_settings.binaryPath << args.join(" ");

		connect(process.get(), &QProcess::finished, this, &RuntimeProbeTask::finished);
		connect(process.get(), &QProcess::errorOccurred, this, &RuntimeProbeTask::error);
		connect(process.get(), &QProcess::readyReadStandardOutput, this, &RuntimeProbeTask::stdoutReady);
		connect(process.get(), &QProcess::readyReadStandardError, this, &RuntimeProbeTask::stderrReady);
		connect(&killTimer, &QTimer::timeout, this, &RuntimeProbeTask::timeout);
		killTimer.setSingleShot(true);
		killTimer.start(15000);
		process->start();
	}

	void RuntimeProbeTask::stdoutReady()
	{
		QByteArray data = process->readAllStandardOutput();
		QString added = QString::fromLocal8Bit(data);
		added.remove('\r');
		m_stdout += added;
	}

	void RuntimeProbeTask::stderrReady()
	{
		QByteArray data = process->readAllStandardError();
		QString added = QString::fromLocal8Bit(data);
		added.remove('\r');
		m_stderr += added;
	}

	void RuntimeProbeTask::finished(int exitcode, QProcess::ExitStatus status)
	{
		killTimer.stop();
		QProcessPtr activeProcess = process;
		process.reset();

		ProbeReport report;
		report.path = m_settings.binaryPath;
		report.token = m_settings.token;
		report.stderrLog = m_stderr;
		report.stdoutLog = m_stdout;
		qDebug() << "STDOUT" << m_stdout;
		if (!m_stderr.isEmpty())
		{
			qWarning() << "STDERR" << m_stderr;
		}
		qDebug() << "Runtime probe finished with status" << status << "exit code" << exitcode;

		if (status == QProcess::CrashExit || exitcode == 1)
		{
			report.status = ProbeReport::Status::Errored;
			emit probeFinished(report);
			emitSucceeded();
			return;
		}

		QMap<QString, QString> results;
		QStringList lines = m_stdout.split("\n", Qt::SkipEmptyParts);
		for (QString line : lines)
		{
			line = line.trimmed();
			if (line.contains("/bedrock/strata"))
			{
				continue;
			}
			auto parts = line.split('=', Qt::SkipEmptyParts);
			if (parts.size() != 2 || parts[0].isEmpty() || parts[1].isEmpty())
			{
				continue;
			}
			results.insert(parts[0], parts[1]);
		}

		if (!results.contains("os.arch") || !results.contains("java.version") || !results.contains("java.vendor"))
		{
			report.status = ProbeReport::Status::InvalidData;
			emit probeFinished(report);
			emitSucceeded();
			return;
		}

		auto osArch = results["os.arch"];
		auto javaVersion = results["java.version"];
		auto javaVendor = results["java.vendor"];
		bool is64 = osArch == "x86_64" || osArch == "amd64" || osArch == "aarch64" || osArch == "arm64" ||
				 osArch == "riscv64";

		report.status = ProbeReport::Status::Valid;
		report.is_64bit = is64;
		report.platformTag = is64 ? "64" : "32";
		report.platformArch = osArch;
		report.version = javaVersion;
		report.vendor = javaVendor;
		qDebug() << "Runtime probe succeeded.";
		emit probeFinished(report);
		emitSucceeded();
	}

	void RuntimeProbeTask::error(QProcess::ProcessError err)
	{
		if (err == QProcess::FailedToStart)
		{
			qDebug() << "Runtime probe failed to start.";
			qDebug() << "Process environment:";
			qDebug() << process->environment();
			qDebug() << "Native environment:";
			qDebug() << QProcessEnvironment::systemEnvironment().toStringList();
			killTimer.stop();
			ProbeReport report;
			report.path = m_settings.binaryPath;
			report.token = m_settings.token;
			emit probeFinished(report);
		}
		emitSucceeded();
	}

	void RuntimeProbeTask::timeout()
	{
		if (process)
		{
			qDebug() << "Runtime probe has been killed by timeout.";
			process->kill();
		}
	}
} // namespace projt::java
