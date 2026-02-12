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

#include <QButtonGroup>
#include <QDialog>

namespace Ui
{
	class ChooseProviderDialog;
}

namespace ModPlatform
{
	enum class ResourceProvider;
}

class Mod;
class NetJob;

class ChooseProviderDialog : public QDialog
{
	Q_OBJECT

	struct Response
	{
		bool skip_all	 = false;
		bool confirm_all = false;

		bool try_others = false;

		ModPlatform::ResourceProvider chosen;
	};

  public:
	explicit ChooseProviderDialog(QWidget* parent, bool single_choice = false, bool allow_skipping = true);
	~ChooseProviderDialog();

	auto getResponse() const -> Response
	{
		return m_response;
	}

	void setDescription(QString desc);

  private slots:
	void skipOne();
	void skipAll();
	void confirmOne();
	void confirmAll();

  private:
	void addProviders();
	void disableInput();

	auto getSelectedProvider() const -> ModPlatform::ResourceProvider;

  private:
	Ui::ChooseProviderDialog* ui;

	QButtonGroup m_providers;

	Response m_response;
};
