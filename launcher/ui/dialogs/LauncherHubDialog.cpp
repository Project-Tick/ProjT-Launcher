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

#include "LauncherHubDialog.h"

#include <QVBoxLayout>

#include "ui/widgets/LauncherHubWidget.h"

LauncherHubDialog::LauncherHubDialog(QWidget* parent) : QDialog(parent)
{
	setWindowTitle(tr("Launcher Hub"));
	setAttribute(Qt::WA_DeleteOnClose);
	resize(1100, 720);

	auto* layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);

	m_widget = new LauncherHubWidget(this);
	layout->addWidget(m_widget);
	m_widget->ensureLoaded();

	connect(m_widget, &LauncherHubWidget::selectInstanceRequested, this, &LauncherHubDialog::selectInstanceRequested);
	connect(m_widget, &LauncherHubWidget::launchInstanceRequested, this, &LauncherHubDialog::launchInstanceRequested);
	connect(m_widget, &LauncherHubWidget::editInstanceRequested, this, &LauncherHubDialog::editInstanceRequested);
	connect(m_widget, &LauncherHubWidget::backupsRequested, this, &LauncherHubDialog::backupsRequested);
	connect(m_widget,
			&LauncherHubWidget::openInstanceFolderRequested,
			this,
			&LauncherHubDialog::openInstanceFolderRequested);
}

LauncherHubDialog::~LauncherHubDialog() = default;

void LauncherHubDialog::openUrl(const QUrl& url)
{
	if (!m_widget)
	{
		return;
	}
	m_widget->ensureLoaded();
	m_widget->openUrl(url);
}

void LauncherHubDialog::setSelectedInstanceId(const QString& id)
{
	if (!m_widget)
	{
		return;
	}
	m_widget->setSelectedInstanceId(id);
}

void LauncherHubDialog::refreshCockpit()
{
	if (!m_widget)
	{
		return;
	}
	m_widget->refreshCockpit();
}
