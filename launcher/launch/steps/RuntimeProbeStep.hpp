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
#include <launch/LaunchStage.hpp>

#include <QString>

class QFileInfo;

namespace projt::launch::steps
{
	class RuntimeProbeStep : public projt::launch::LaunchStage
	{
		Q_OBJECT
	  public:
		explicit RuntimeProbeStep(projt::launch::LaunchPipeline* parent);
		~RuntimeProbeStep() override = default;

		void executeTask() override;
		bool canAbort() const override
		{
			return false;
		}

	  private slots:
		void onProbeFinished(const projt::java::RuntimeProbeTask::ProbeReport& report);

	  private:
		struct CachedRuntimeInfo
		{
			QString signature;
			QString version;
			QString arch;
			QString realArch;
			QString vendor;

			bool isComplete() const;
		};

		CachedRuntimeInfo readCache() const;
		QString computeSignature(const QFileInfo& info, const QString& resolvedPath) const;
		bool needsProbe(const CachedRuntimeInfo& cache, const QString& signature) const;
		void storeProbeResult(const projt::java::RuntimeProbeTask::ProbeReport& report) const;
		void printRuntimeInfo(const QString& version,
							  const QString& architecture,
							  const QString& realArchitecture,
							  const QString& vendor);

		QString m_resolvedPath;
		QString m_signature;
		projt::java::RuntimeProbeTask::Ptr m_probeTask;
	};
} // namespace projt::launch::steps
