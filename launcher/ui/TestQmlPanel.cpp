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

#include "TestQmlPanel.h"

#include <QQmlContext>
#include <QQuickWidget>
#include <QUrl>
#include <QVBoxLayout>

#include "viewmodels/LauncherViewModel.h"

TestQmlPanel::TestQmlPanel(LauncherViewModel* viewModel, QWidget* parent)
    : QDockWidget(parent)
{
    setObjectName(QStringLiteral("TestQmlPanelDock"));
    setWindowTitle(tr("QML Preview"));
    setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);

    auto container = new QWidget(this);
    auto layout = new QVBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 0);

    m_quickWidget = new QQuickWidget(container);
    m_quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    m_quickWidget->setClearColor(Qt::transparent);
    if (viewModel) {
        m_quickWidget->rootContext()->setContextProperty(QStringLiteral("launcherViewModel"), viewModel);
    }
    m_quickWidget->setSource(QUrl(QStringLiteral("qrc:/qml/TestShell.qml")));

    layout->addWidget(m_quickWidget);
    setWidget(container);
}
