// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
 *
 *  This file is part of ProjT Launcher and is licensed under
 *  the GNU General Public License version 3 or later.
 *
 *  If this file includes work from previous open-source projects,
 *  their original copyright and license notices are preserved below.
 */
#include <QCloseEvent>

#include "ViewLogWindow.h"

#include "ui/pages/instance/OtherLogsPage.h"
#include "viewmodels/LogsViewModel.h"

ViewLogWindow::ViewLogWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_logsViewModel(new LogsViewModel(this))
    , m_page(new OtherLogsPage("launcher-logs", tr("Launcher Logs"), "Launcher-Logs", nullptr, parent))
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowIcon(QIcon::fromTheme("log"));
    setWindowTitle(tr("View Launcher Logs"));
    setCentralWidget(m_page);
    setMinimumSize(m_page->size());
    setContentsMargins(0, 0, 0, 0);
    m_page->setLogsViewModel(m_logsViewModel);
    m_page->opened();
    show();
}

void ViewLogWindow::closeEvent(QCloseEvent* event)
{
    m_page->closed();
    emit isClosing();
    event->accept();
}
