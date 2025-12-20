// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
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
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "TestQmlPanel.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QQmlContext>
#include <QQuickWidget>
#include <QUrl>
#include <QVBoxLayout>

#include "viewmodels/LauncherViewModel.h"

namespace {
QUrl resolveQmlUrl(const QString& fileName)
{
    // First try the embedded resource
    const QString resourcePath = QStringLiteral(":/qml/%1").arg(fileName);
    if (QFile::exists(resourcePath)) {
        qDebug() << "[TestQmlPanel] Loading QML from resource:" << resourcePath;
        return QUrl(QStringLiteral("qrc:/qml/%1").arg(fileName));
    }

    // Try to find source directory for development builds
    QDir dir(QCoreApplication::applicationDirPath());

    // Try going up multiple levels to find source tree
    for (int i = 0; i < 4; ++i) {
        QDir sourceDir(dir);
        if (sourceDir.cd(QStringLiteral("launcher/ui/qml"))) {
            QFileInfo info(sourceDir.filePath(fileName));
            if (info.exists()) {
                qDebug() << "[TestQmlPanel] Loading QML from source:" << info.absoluteFilePath();
                return QUrl::fromLocalFile(info.absoluteFilePath());
            }
        }
        if (!dir.cdUp())
            break;
    }

    qWarning() << "[TestQmlPanel] QML file not found:" << fileName << "- trying qrc anyway";
    return QUrl(QStringLiteral("qrc:/qml/%1").arg(fileName));
}
}  // namespace

TestQmlPanel::TestQmlPanel(LauncherViewModel* viewModel, QWidget* parent) : QDockWidget(parent)
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
    m_quickWidget->setSource(resolveQmlUrl(QStringLiteral("TestShell.qml")));

    layout->addWidget(m_quickWidget);
    setWidget(container);
}
