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

#include <QProcess>
#include <QTimer>

#include "QObjectPtr.h"
#include "java/core/RuntimeVersion.hpp"
#include "tasks/Task.h"

namespace projt::java
{
	class RuntimeProbeTask : public Task
	{
		Q_OBJECT
	  public:
		using QProcessPtr = shared_qobject_ptr<QProcess>;
		using Ptr		  = shared_qobject_ptr<RuntimeProbeTask>;

		struct ProbeSettings
		{
			QString binaryPath;
			QString extraArgs;
			int minMem	= 0;
			int maxMem	= 0;
			int permGen = 0;
			int token	= 0;
		};

		struct ProbeReport
		{
			QString path;
			int token = 0;
			QString platformTag;
			QString platformArch;
			RuntimeVersion version;
			QString vendor;
			QString stdoutLog;
			QString stderrLog;
			bool is_64bit = false;
			enum class Status
			{
				Errored,
				InvalidData,
				Valid
			} status = Status::Errored;
		};

		explicit RuntimeProbeTask(ProbeSettings settings);

		static QString probeJarPath();

	  signals:
		void probeFinished(const ProbeReport& report);

	  protected:
		void executeTask() override;

	  private:
		QProcessPtr process;
		QTimer killTimer;
		QString m_stdout;
		QString m_stderr;

		ProbeSettings m_settings;

	  private slots:
		void timeout();
		void finished(int exitcode, QProcess::ExitStatus status);
		void error(QProcess::ProcessError err);
		void stdoutReady();
		void stderrReady();
	};
} // namespace projt::java