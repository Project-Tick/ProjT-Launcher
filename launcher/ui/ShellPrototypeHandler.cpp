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

#include "ShellPrototypeHandler.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QCoreApplication>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQmlPropertyMap>
#include <QQuickWidget>
#include <QUrl>
#include <QVBoxLayout>

#include "Application.h"
#include "settings/Setting.h"
#include "settings/SettingsObject.h"
#include "viewmodels/InstanceListViewModel.h"
#include "viewmodels/LauncherViewModel.h"
#include "viewmodels/NewsViewModel.h"
#include "viewmodels/SettingsViewModel.h"

constexpr auto kShellLastPageSetting = "qmlShell/lastPage";
constexpr auto kShellDockVisibleSetting = "qmlShell/dockVisible";
constexpr auto kShellSidebarWidthSetting = "qmlShell/sidebarWidth";

class ShellStateBridge : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString lastPageRoute READ lastPageRoute WRITE setLastPageRoute NOTIFY lastPageRouteChanged)
    Q_PROPERTY(int sidebarWidth READ sidebarWidth WRITE setSidebarWidth NOTIFY sidebarWidthChanged)
    Q_PROPERTY(bool dockVisible READ dockVisible WRITE setDockVisible NOTIFY dockVisibleChanged)

   public:
    explicit ShellStateBridge(SettingsObjectPtr settings, QObject* parent = nullptr)
        : QObject(parent), m_settings(std::move(settings))
    {
        if (m_settings) {
            m_lastPage = m_settings->getOrRegisterSetting(QString::fromLatin1(kShellLastPageSetting),
                                                          QStringLiteral("instances"))->get().toString();
            m_sidebarWidth = m_settings->getOrRegisterSetting(QString::fromLatin1(kShellSidebarWidthSetting), 200)->get().toInt();
            m_dockVisible =
                m_settings->getOrRegisterSetting(QString::fromLatin1(kShellDockVisibleSetting), false)->get().toBool();
        }
    }

    QString lastPageRoute() const { return m_lastPage; }
    int sidebarWidth() const { return m_sidebarWidth; }
    bool dockVisible() const { return m_dockVisible; }

   public slots:
    void setLastPageRoute(const QString& route)
    {
        if (route.isEmpty() || route == m_lastPage) {
            return;
        }
        m_lastPage = route;
        persist(QString::fromLatin1(kShellLastPageSetting), m_lastPage);
        emit lastPageRouteChanged();
    }

    void setSidebarWidth(int width)
    {
        if (width <= 0 || width == m_sidebarWidth) {
            return;
        }
        m_sidebarWidth = width;
        persist(QString::fromLatin1(kShellSidebarWidthSetting), m_sidebarWidth);
        emit sidebarWidthChanged();
    }

    void setDockVisible(bool visible)
    {
        if (visible == m_dockVisible) {
            return;
        }
        m_dockVisible = visible;
        persist(QString::fromLatin1(kShellDockVisibleSetting), m_dockVisible);
        emit dockVisibleChanged();
    }

   signals:
    void lastPageRouteChanged();
    void sidebarWidthChanged();
    void dockVisibleChanged();

   private:
    void persist(const QString& key, const QVariant& value)
    {
        if (!m_settings) {
            return;
        }
        if (auto setting = m_settings->getSetting(key)) {
            setting->set(value);
        }
    }

    SettingsObjectPtr m_settings;
    QString m_lastPage = QStringLiteral("instances");
    int m_sidebarWidth = 200;
    bool m_dockVisible = false;
};

static QUrl resolveQmlUrl(const QString& fileName)
{
    const QString resourcePath = QStringLiteral(":/qml/%1").arg(fileName);
    if (QFile::exists(resourcePath)) {
        return QUrl(QStringLiteral("qrc:/qml/%1").arg(fileName));
    }
    QDir dir(QCoreApplication::applicationDirPath());
    if (dir.cdUp() && dir.cd(QStringLiteral("launcher/qml"))) {
        QFileInfo info(dir.filePath(fileName));
        if (info.exists()) {
            return QUrl::fromLocalFile(info.absoluteFilePath());
        }
    }
    return QUrl(QStringLiteral("qrc:/qml/%1").arg(fileName));
}

namespace {
bool registerLauncherViewModelEnums()
{
    qmlRegisterUncreatableMetaObject(LauncherViewModel::staticMetaObject, "ProjTLauncher", 1, 0, "LauncherViewModelEnums",
                                     QStringLiteral("Enums are exposed via existing context objects."));
    return true;
}

const bool s_launcherVmEnumsRegistered = registerLauncherViewModelEnums();
}  // namespace

ShellPrototypeHandler::ShellPrototypeHandler(LauncherViewModel* launcherViewModel, InstanceListViewModel* instanceListViewModel,
                                             NewsViewModel* newsViewModel, SettingsViewModel* settingsViewModel, QWidget* parent)
    : QDockWidget(parent)
{
    setObjectName(QStringLiteral("ShellPrototypeDock"));
    setWindowTitle(tr("QML Shell Prototype"));
    setAllowedAreas(Qt::AllDockWidgetAreas);

    auto container = new QWidget(this);
    auto layout = new QVBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 0);

    m_quickWidget = new QQuickWidget(container);
    m_quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    m_quickWidget->setClearColor(Qt::transparent);

    exposeContextProperties(launcherViewModel, instanceListViewModel, newsViewModel, settingsViewModel, APPLICATION->settings());

    m_quickWidget->setSource(resolveQmlUrl(QStringLiteral("ShellRoot.qml")));

    layout->addWidget(m_quickWidget);
    setWidget(container);

    connect(this, &QDockWidget::visibilityChanged, this, [this](bool visible) {
        if (m_stateBridge) {
            m_stateBridge->setDockVisible(visible);
        }
    });
}

void ShellPrototypeHandler::exposeContextProperties(LauncherViewModel* launcherViewModel,
                                                    InstanceListViewModel* instanceListViewModel,
                                                    NewsViewModel* newsViewModel,
                                                    SettingsViewModel* settingsViewModel,
                                                    SettingsObjectPtr settings)
{
    auto ctx = m_quickWidget->rootContext();

    m_stateBridge = new ShellStateBridge(settings, this);
    ctx->setContextProperty(QStringLiteral("shellState"), m_stateBridge);

    auto projt = new QQmlPropertyMap(this);
    projt->insert(QStringLiteral("launcherVM"), QVariant::fromValue(launcherViewModel));
    projt->insert(QStringLiteral("instancesVM"), QVariant::fromValue(instanceListViewModel));
    projt->insert(QStringLiteral("newsVM"), QVariant::fromValue(newsViewModel));
    projt->insert(QStringLiteral("settingsVM"), QVariant::fromValue(settingsViewModel));
    ctx->setContextProperty(QStringLiteral("ProjT"), projt);

    if (launcherViewModel) {
        ctx->setContextProperty(QStringLiteral("launcherVM"), launcherViewModel);
        ctx->setContextProperty(QStringLiteral("launcherViewModel"), launcherViewModel);
    }
    if (instanceListViewModel) {
        ctx->setContextProperty(QStringLiteral("instancesVM"), instanceListViewModel);
        ctx->setContextProperty(QStringLiteral("instanceListViewModel"), instanceListViewModel);
    }
    if (newsViewModel) {
        ctx->setContextProperty(QStringLiteral("newsVM"), newsViewModel);
        ctx->setContextProperty(QStringLiteral("newsViewModel"), newsViewModel);
    }
    if (settingsViewModel) {
        ctx->setContextProperty(QStringLiteral("settingsVM"), settingsViewModel);
        ctx->setContextProperty(QStringLiteral("settingsViewModel"), settingsViewModel);
    }

    if (launcherViewModel && m_stateBridge) {
        launcherViewModel->setCurrentPage(LauncherViewModel::stringToPage(m_stateBridge->lastPageRoute()));
        connect(m_stateBridge, &ShellStateBridge::lastPageRouteChanged, launcherViewModel, [this, launcherViewModel]() {
            launcherViewModel->setCurrentPage(LauncherViewModel::stringToPage(m_stateBridge->lastPageRoute()));
        });
        connect(launcherViewModel, &LauncherViewModel::currentPageChanged, this, [this, launcherViewModel]() {
            if (m_stateBridge) {
                m_stateBridge->setLastPageRoute(LauncherViewModel::pageToString(launcherViewModel->currentPage()));
            }
        });
    }

    if (m_stateBridge && m_stateBridge->dockVisible()) {
        show();
    }
}

#include "ShellPrototypeHandler.moc"
