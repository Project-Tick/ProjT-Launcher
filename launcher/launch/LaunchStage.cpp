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

#include "LaunchStage.hpp"
#include "LaunchPipeline.hpp"

namespace projt::launch
{
	LaunchStage::LaunchStage(LaunchPipeline* pipeline) : Task(), m_flow(pipeline)
	{
		if (!m_flow)
		{
			return;
		}
		connect(this, &LaunchStage::readyForLaunch, m_flow, &LaunchPipeline::onReadyForLaunch);
		connect(this, &LaunchStage::logLine, m_flow, &LaunchPipeline::onLogLine);
		connect(this, &LaunchStage::logLines, m_flow, &LaunchPipeline::onLogLines);
		connect(this, &LaunchStage::finished, m_flow, &LaunchPipeline::onStageFinished);
		connect(this, &LaunchStage::progressReportingRequest, m_flow, &LaunchPipeline::onProgressReportingRequested);
	}
} // namespace projt::launch
