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
#include <QCloseEvent>

#include "ViewLogWindow.h"

#include "ui/pages/instance/OtherLogsPage.h"

ViewLogWindow::ViewLogWindow(QWidget* parent)
	: QMainWindow(parent),
	  m_page(new OtherLogsPage("launcher-logs", tr("Launcher Logs"), "Launcher-Logs", nullptr, parent))
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowIcon(QIcon::fromTheme("log"));
	setWindowTitle(tr("View Launcher Logs"));
	setCentralWidget(m_page);
	setMinimumSize(m_page->size());
	setContentsMargins(0, 0, 0, 0);
	m_page->opened();
	show();
}

void ViewLogWindow::closeEvent(QCloseEvent* event)
{
	m_page->closed();
	emit isClosing();
	event->accept();
}
