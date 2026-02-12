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

#include "RuntimeProbeStep.hpp"

#include <FileSystem.h>
#include <launch/LaunchPipeline.hpp>

#include <QCryptographicHash>
#include <QFileInfo>
#include <QStandardPaths>

namespace projt::launch::steps
{
	RuntimeProbeStep::RuntimeProbeStep(projt::launch::LaunchPipeline* parent) : projt::launch::LaunchStage(parent)
	{}

	bool RuntimeProbeStep::CachedRuntimeInfo::isComplete() const
	{
		return !version.isEmpty() && !arch.isEmpty() && !realArch.isEmpty() && !vendor.isEmpty();
	}

	RuntimeProbeStep::CachedRuntimeInfo RuntimeProbeStep::readCache() const
	{
		auto settings = m_flow->instance()->settings();
		CachedRuntimeInfo cache;
		cache.signature = settings->get("JavaSignature").toString();
		cache.version	= settings->get("JavaVersion").toString();
		cache.arch		= settings->get("JavaArchitecture").toString();
		cache.realArch	= settings->get("JavaRealArchitecture").toString();
		cache.vendor	= settings->get("JavaVendor").toString();
		return cache;
	}

	QString RuntimeProbeStep::computeSignature(const QFileInfo& info, const QString& resolvedPath) const
	{
		QCryptographicHash hash(QCryptographicHash::Sha1);
		hash.addData(QByteArray::number(info.lastModified().toMSecsSinceEpoch()));
		hash.addData(resolvedPath.toUtf8());
		return hash.result().toHex();
	}

	bool RuntimeProbeStep::needsProbe(const CachedRuntimeInfo& cache, const QString& signature) const
	{
		if (cache.signature != signature)
		{
			return true;
		}
		return !cache.isComplete();
	}

	void RuntimeProbeStep::executeTask()
	{
		auto instance = m_flow->instance();
		auto settings = instance->settings();

		QString configuredPath = settings->get("JavaPath").toString();
		m_resolvedPath		   = FS::ResolveExecutable(configuredPath);

		bool perInstance = settings->get("OverrideJava").toBool() || settings->get("OverrideJavaLocation").toBool();

		QString executablePath = QStandardPaths::findExecutable(m_resolvedPath);
		if (executablePath.isEmpty())
		{
			if (perInstance)
			{
				emit logLine(
					QString("The Java binary \"%1\" couldn't be found. Please fix the Java path override in the "
							"instance's settings or disable it.")
						.arg(configuredPath),
					MessageLevel::Warning);
			}
			else
			{
				emit logLine(QString("The Java binary \"%1\" couldn't be found. Please set up Java in the settings.")
								 .arg(configuredPath),
							 MessageLevel::Warning);
			}
			emitFailed(QString("Java path is not valid."));
			return;
		}

		emit logLine("Java path is:\n" + m_resolvedPath + "\n\n", MessageLevel::Launcher);

		if (projt::java::RuntimeProbeTask::probeJarPath().isEmpty())
		{
			const char* reason = QT_TR_NOOP("Java checker library could not be found. Please check your installation.");
			emit logLine(tr(reason), MessageLevel::Fatal);
			emitFailed(tr(reason));
			return;
		}

		QFileInfo javaInfo(executablePath);
		m_signature = computeSignature(javaInfo, m_resolvedPath);

		auto cache = readCache();
		if (!needsProbe(cache, m_signature))
		{
			printRuntimeInfo(cache.version, cache.arch, cache.realArch, cache.vendor);
			instance->updateRuntimeContext();
			emitSucceeded();
			return;
		}

		projt::java::RuntimeProbeTask::ProbeSettings probeSettings;
		probeSettings.binaryPath = executablePath;
		m_probeTask.reset(new projt::java::RuntimeProbeTask(probeSettings));
		emit logLine(tr("Checking Java version..."), MessageLevel::Launcher);
		connect(m_probeTask.get(),
				&projt::java::RuntimeProbeTask::probeFinished,
				this,
				&RuntimeProbeStep::onProbeFinished);
		m_probeTask->start();
	}

	void RuntimeProbeStep::onProbeFinished(const projt::java::RuntimeProbeTask::ProbeReport& report)
	{
		switch (report.status)
		{
			case projt::java::RuntimeProbeTask::ProbeReport::Status::Errored:
			{
				emit logLine(QString("Could not start java:"), MessageLevel::Error);
				emit logLines(report.stderrLog.split('\n'), MessageLevel::Error);
				emit logLine(QString("\nCheck your Java settings."), MessageLevel::Launcher);
				emitFailed(QString("Could not start java!"));
				return;
			}
			case projt::java::RuntimeProbeTask::ProbeReport::Status::InvalidData:
			{
				emit logLine(QString("Java checker returned some invalid data we don't understand:"),
							 MessageLevel::Error);
				emit logLines(report.stdoutLog.split('\n'), MessageLevel::Warning);
				emit logLine("\nMinecraft might not start properly.", MessageLevel::Launcher);
				m_flow->instance()->updateRuntimeContext();
				emitSucceeded();
				return;
			}
			case projt::java::RuntimeProbeTask::ProbeReport::Status::Valid:
			{
				printRuntimeInfo(report.version.toString(), report.platformTag, report.platformArch, report.vendor);
				storeProbeResult(report);
				m_flow->instance()->updateRuntimeContext();
				emitSucceeded();
				return;
			}
		}
	}

	void RuntimeProbeStep::storeProbeResult(const projt::java::RuntimeProbeTask::ProbeReport& report) const
	{
		auto settings = m_flow->instance()->settings();
		settings->set("JavaVersion", report.version.toString());
		settings->set("JavaArchitecture", report.platformTag);
		settings->set("JavaRealArchitecture", report.platformArch);
		settings->set("JavaVendor", report.vendor);
		settings->set("JavaSignature", m_signature);
	}

	void RuntimeProbeStep::printRuntimeInfo(const QString& version,
											const QString& architecture,
											const QString& realArchitecture,
											const QString& vendor)
	{
		emit logLine(QString("Java is version %1, using %2 (%3) architecture, from %4.\n\n")
						 .arg(version, architecture, realArchitecture, vendor),
					 MessageLevel::Launcher);
	}
} // namespace projt::launch::steps
