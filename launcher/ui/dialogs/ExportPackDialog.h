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
 *
 * === Upstream License Block (Do Not Modify) ==============================
 *
 *
 *
 *  Prism Launcher - Minecraft Launcher
 *  Copyright (C) 2023 TheKodeToad <TheKodeToad@proton.me>
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
 *
 * ======================================================================== */

#pragma once

#include <QDialog>
#include "BaseInstance.h"
#include "minecraft/MinecraftInstance.h"
#include "modplatform/ModIndex.h"
#include "ui/widgets/FastFileIconProvider.h"
#include "ui/widgets/FileIgnoreProxy.h"

namespace Ui
{
	class ExportPackDialog;
}

class ExportPackDialog : public QDialog
{
	Q_OBJECT

  public:
	explicit ExportPackDialog(MinecraftInstancePtr instance,
							  QWidget* parent						 = nullptr,
							  ModPlatform::ResourceProvider provider = ModPlatform::ResourceProvider::MODRINTH);
	~ExportPackDialog();

	void done(int result) override;
	void validate();

  private:
	QString ignoreFileName();

  private:
	const MinecraftInstancePtr m_instance;
	Ui::ExportPackDialog* m_ui;
	FileIgnoreProxy* m_proxy;
	FastFileIconProvider m_icons;
	const ModPlatform::ResourceProvider m_provider;
};
