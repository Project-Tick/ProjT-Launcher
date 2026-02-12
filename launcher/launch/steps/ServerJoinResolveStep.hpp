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

#include <launch/LaunchStage.hpp>
#include <minecraft/launch/MinecraftTarget.hpp>

#include <QPointer>
#include <QString>

class QDnsLookup;

namespace projt::launch::steps
{
	class ServerJoinResolveStep : public projt::launch::LaunchStage
	{
		Q_OBJECT
	  public:
		explicit ServerJoinResolveStep(projt::launch::LaunchPipeline* parent);
		~ServerJoinResolveStep() override = default;

		void executeTask() override;
		bool abort() override;
		bool canAbort() const override
		{
			return true;
		}

		void setLookupAddress(const QString& address);
		void setOutputTarget(MinecraftTarget::Ptr target);

	  private slots:
		void onLookupFinished();

	  private:
		void finalizeTarget(const QString& address, quint16 port);

		QPointer<QDnsLookup> m_lookup;
		QString m_lookupAddress;
		QString m_queryName;
		MinecraftTarget::Ptr m_output;
	};
} // namespace projt::launch::steps
