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
#include <java/services/RuntimeProbeTask.hpp>

class QWidget;

/**
 * Common UI bits for the java pages to use.
 */
namespace JavaCommon
{
	bool checkJVMArgs(QString args, QWidget* parent);

	// Show a dialog saying that the Java binary was usable
	void javaWasOk(QWidget* parent, const projt::java::RuntimeProbeTask::ProbeReport& result);
	// Show a dialog saying that the Java binary was not usable because of bad options
	void javaArgsWereBad(QWidget* parent, const projt::java::RuntimeProbeTask::ProbeReport& result);
	// Show a dialog saying that the Java binary was not usable
	void javaBinaryWasBad(QWidget* parent, const projt::java::RuntimeProbeTask::ProbeReport& result);
	// Show a dialog if we couldn't find Java Checker
	void javaCheckNotFound(QWidget* parent);

	class TestCheck : public QObject
	{
		Q_OBJECT
	  public:
		TestCheck(QWidget* parent, QString path, QString args, int minMem, int maxMem, int permGen)
			: m_parent(parent),
			  m_path(path),
			  m_args(args),
			  m_minMem(minMem),
			  m_maxMem(maxMem),
			  m_permGen(permGen)
		{}
		virtual ~TestCheck() = default;

		void run();

	  signals:
		void finished();

	  private slots:
		void checkFinished(const projt::java::RuntimeProbeTask::ProbeReport& result);
		void checkFinishedWithArgs(const projt::java::RuntimeProbeTask::ProbeReport& result);

	  private:
		projt::java::RuntimeProbeTask::Ptr checker;
		QWidget* m_parent = nullptr;
		QString m_path;
		QString m_args;
		int m_minMem  = 0;
		int m_maxMem  = 0;
		int m_permGen = 64;
	};
} // namespace JavaCommon
