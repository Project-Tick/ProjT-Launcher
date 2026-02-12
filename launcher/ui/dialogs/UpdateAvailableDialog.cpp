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
 *
 *
 *
 *  Prism Launcher - Minecraft Launcher
 *  Copyright (C) 2023 Rachel Powers <508861+Ryex@users.noreply.github.com>
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
 *
 *
 * ======================================================================== */

#include "UpdateAvailableDialog.h"
#include <QPushButton>
#include "BuildConfig.h"
#include "Markdown.h"
#include "StringUtils.h"
#include "ui_UpdateAvailableDialog.h"

UpdateAvailableDialog::UpdateAvailableDialog(const QString& currentVersion,
											 const QString& availableVersion,
											 const QString& releaseNotes,
											 Mode mode,
											 QWidget* parent)
	: QDialog(parent),
	  ui(new Ui::UpdateAvailableDialog)
{
	ui->setupUi(this);

	QString launcherName = BuildConfig.LAUNCHER_DISPLAYNAME;

	if (mode == Mode::Migration)
	{
		ui->headerLabel->setText(tr("A new release line of %1 is available!").arg(launcherName));
		ui->versionAvailableLabel->setText(tr("Version %1 is part of a new release line.\n"
											  "You are currently on %2. Would you like to migrate now?")
											   .arg(availableVersion)
											   .arg(currentVersion));
	}
	else
	{
		ui->headerLabel->setText(tr("A new version of %1 is available!").arg(launcherName));
		ui->versionAvailableLabel->setText(
			tr("Version %1 is now available - you have %2 . Would you like to download it now?")
				.arg(availableVersion)
				.arg(currentVersion));
	}
	ui->icon->setPixmap(QIcon::fromTheme("checkupdate").pixmap(64));

	auto releaseNotesHtml = markdownToHTML(releaseNotes);
	ui->releaseNotes->setHtml(StringUtils::htmlListPatch(releaseNotesHtml));
	ui->releaseNotes->setOpenExternalLinks(true);

	connect(ui->skipButton,
			&QPushButton::clicked,
			this,
			[this]()
			{
				setResult(ResultCode::Skip);
				done(ResultCode::Skip);
			});

	connect(ui->delayButton,
			&QPushButton::clicked,
			this,
			[this]()
			{
				setResult(ResultCode::DontInstall);
				done(ResultCode::DontInstall);
			});

	connect(ui->installButton,
			&QPushButton::clicked,
			this,
			[this]()
			{
				setResult(ResultCode::Install);
				done(ResultCode::Install);
			});
}
