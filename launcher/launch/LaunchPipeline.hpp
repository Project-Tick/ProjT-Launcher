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

#include <QObjectPtr.h>
#include <minecraft/MinecraftInstance.h>
#include <QList>
#include <QMap>
#include <QProcessEnvironment>

#include "LaunchLineRouter.hpp"
#include "LaunchLogModel.hpp"
#include "LaunchStage.hpp"
#include "LaunchVariableExpander.hpp"
#include "MessageLevel.h"

class BaseInstance;

namespace projt::launch
{
	class LaunchPipeline : public Task
	{
		Q_OBJECT
	  public:
		using Ptr	   = shared_qobject_ptr<LaunchPipeline>;
		using StagePtr = shared_qobject_ptr<LaunchStage>;

		enum class State
		{
			Idle,
			Running,
			Waiting,
			Failed,
			Aborted,
			Finished
		};

		static Ptr create(MinecraftInstancePtr instance);
		explicit LaunchPipeline(MinecraftInstancePtr instance);
		~LaunchPipeline() override = default;

		void appendStage(StagePtr stage);
		void prependStage(StagePtr stage);
		void setCensorFilter(QMap<QString, QString> filter);

		MinecraftInstancePtr instance() const
		{
			return m_instanceRef;
		}

		void setPid(qint64 pid)
		{
			m_processId = pid;
		}

		qint64 pid() const
		{
			return m_processId;
		}

		void executeTask() override;
		void proceed();
		bool abort() override;
		bool canAbort() const override;

		shared_qobject_ptr<LaunchLogModel> logModel();

		QString substituteVariables(const QString& cmd, bool isLaunch = false) const;
		QString censorPrivateInfo(QString input) const;

	  signals:
		void readyForLaunch();
		void requestProgress(Task* task);

	  public slots:
		void onLogLines(const QStringList& lines, MessageLevel::Enum defaultLevel = MessageLevel::Launcher);
		void onLogLine(QString line, MessageLevel::Enum defaultLevel = MessageLevel::Launcher);
		void onReadyForLaunch();
		void onStageFinished();
		void onProgressReportingRequested();

	  protected:
		void emitFailed(QString reason) override;
		void emitSucceeded() override;

	  private:
		void advanceStage();
		void closeStages(bool successful, const QString& error);

		MinecraftInstancePtr m_instanceRef;
		shared_qobject_ptr<LaunchLogModel> m_logStore;
		QList<StagePtr> m_stageQueue;
		QList<StagePtr> m_stageHistory;
		StagePtr m_activeStage;
		QMap<QString, QString> m_redactions;
		State m_state	   = State::Idle;
		qint64 m_processId = -1;
		LaunchLineRouter m_lineRouter;
		LaunchVariableExpander m_expander;
	};
} // namespace projt::launch
