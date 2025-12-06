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

#include "QmlMainWindow.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QLibraryInfo>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQmlPropertyMap>
#include <QQuickWidget>
#include <QUrl>
#include <QVBoxLayout>

#include "Application.h"
#include "settings/Setting.h"
#include "settings/SettingsObject.h"
#include "translations/TranslationsModel.h"
#include "viewmodels/ATLauncherViewModel.h"
#include "viewmodels/AccountsViewModel.h"
#include "viewmodels/CurseForgeViewModel.h"
#include "viewmodels/FTBViewModel.h"
#include "viewmodels/InstanceListViewModel.h"
#include "viewmodels/InstanceViewModel.h"
#include "viewmodels/LauncherSettingsViewModel.h"
#include "viewmodels/LauncherViewModel.h"
#include "viewmodels/LogsViewModel.h"
#include "viewmodels/ModrinthViewModel.h"
#include "viewmodels/NewInstanceViewModel.h"
#include "viewmodels/NewsViewModel.h"
#include "viewmodels/SettingsViewModel.h"
#include "viewmodels/TechnicViewModel.h"
#include "viewmodels/ThemeViewModel.h"

constexpr auto kShellLastPageSetting = "qmlShell/lastPage";
constexpr auto kShellDockVisibleSetting = "qmlShell/dockVisible";
constexpr auto kShellSidebarWidthSetting = "qmlShell/sidebarWidth";

class ShellStateBridge : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString lastPageRoute READ lastPageRoute WRITE setLastPageRoute NOTIFY lastPageRouteChanged)
    Q_PROPERTY(int sidebarWidth READ sidebarWidth WRITE setSidebarWidth NOTIFY sidebarWidthChanged)
    Q_PROPERTY(bool dockVisible READ dockVisible WRITE setDockVisible NOTIFY dockVisibleChanged)

   public:
    explicit ShellStateBridge(SettingsObjectPtr settings, QObject* parent = nullptr) : QObject(parent), m_settings(std::move(settings))
    {
        if (m_settings) {
            m_lastPage =
                m_settings->getOrRegisterSetting(QString::fromLatin1(kShellLastPageSetting), QStringLiteral("instances"))->get().toString();
            m_sidebarWidth = m_settings->getOrRegisterSetting(QString::fromLatin1(kShellSidebarWidthSetting), 200)->get().toInt();
            m_dockVisible = m_settings->getOrRegisterSetting(QString::fromLatin1(kShellDockVisibleSetting), false)->get().toBool();
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
    // First try the embedded resource
    const QString resourcePath = QStringLiteral(":/qml/%1").arg(fileName);
    if (QFile::exists(resourcePath)) {
        qDebug() << "[QmlMainWindow] Loading QML from resource:" << resourcePath;
        return QUrl(QStringLiteral("qrc:/qml/%1").arg(fileName));
    }

    // Try to find source directory for development builds
    // Build path is typically: <project>/build/Debug/projtlauncher.exe
    // Source path is: <project>/launcher/qml/
    QDir dir(QCoreApplication::applicationDirPath());

    // Try going up multiple levels to find source tree
    for (int i = 0; i < 4; ++i) {
        QDir sourceDir(dir);
        if (sourceDir.cd(QStringLiteral("launcher/qml"))) {
            QFileInfo info(sourceDir.filePath(fileName));
            if (info.exists()) {
                qDebug() << "[QmlMainWindow] Loading QML from source:" << info.absoluteFilePath();
                return QUrl::fromLocalFile(info.absoluteFilePath());
            }
        }
        if (!dir.cdUp())
            break;
    }

    qWarning() << "[QmlMainWindow] QML file not found:" << fileName << "- trying qrc anyway";
    return QUrl(QStringLiteral("qrc:/qml/%1").arg(fileName));
}

/**
 * Setup QML import paths for all platforms (Windows, Linux, macOS)
 * This ensures QtQuick modules can be found when loading QML from source files
 */
static void setupQmlImportPaths(QQmlEngine* engine)
{
    if (!engine)
        return;

    // Find Qt library and plugin directories
    QString qtLibPath = QLibraryInfo::path(QLibraryInfo::LibrariesPath);
    QString qtPluginPath = QLibraryInfo::path(QLibraryInfo::PluginsPath);
    QString qtBinPath = QLibraryInfo::path(QLibraryInfo::BinariesPath);

    // 1. Update PATH environment variable to find Qt DLLs/libraries
#ifdef Q_OS_WIN
    QString pathEnv = qEnvironmentVariable("PATH");
    if (!qtBinPath.isEmpty() && QDir(qtBinPath).exists() && !pathEnv.contains(qtBinPath)) {
        pathEnv = qtBinPath + QChar(';') + pathEnv;
        qputenv("PATH", pathEnv.toUtf8());
        qDebug() << "[QmlMainWindow] Updated PATH with Qt bin:" << qtBinPath;
    }
    if (!qtLibPath.isEmpty() && QDir(qtLibPath).exists() && !pathEnv.contains(qtLibPath)) {
        pathEnv = qtLibPath + QChar(';') + pathEnv;
        qputenv("PATH", pathEnv.toUtf8());
        qDebug() << "[QmlMainWindow] Updated PATH with Qt lib:" << qtLibPath;
    }
#else
    QString ldPath = qEnvironmentVariable("LD_LIBRARY_PATH");
    if (!qtLibPath.isEmpty() && QDir(qtLibPath).exists() && !ldPath.contains(qtLibPath)) {
        ldPath = qtLibPath + QChar(':') + ldPath;
        qputenv("LD_LIBRARY_PATH", ldPath.toUtf8());
        qDebug() << "[QmlMainWindow] Updated LD_LIBRARY_PATH with:" << qtLibPath;
    }
#endif

    // 2. Set QT_QPA_PLATFORM_PLUGIN_PATH for platform plugin
    if (!qtPluginPath.isEmpty() && QDir(qtPluginPath).exists()) {
        qputenv("QT_QPA_PLATFORM_PLUGIN_PATH", qtPluginPath.toUtf8());
        qDebug() << "[QmlMainWindow] Set QT_QPA_PLATFORM_PLUGIN_PATH:" << qtPluginPath;
    }

    // 3. Qt's official QML import path from QLibraryInfo
    QString qtQmlPath = QLibraryInfo::path(QLibraryInfo::QmlImportsPath);
    if (!qtQmlPath.isEmpty() && QDir(qtQmlPath).exists()) {
        engine->addImportPath(qtQmlPath);
        qDebug() << "[QmlMainWindow] Added Qt QML import path:" << qtQmlPath;
    }

    // 4. Try to find Qt installation from Qt plugin path
    QString pluginPath = QLibraryInfo::path(QLibraryInfo::PluginsPath);
    if (!pluginPath.isEmpty()) {
        QDir pluginDir(pluginPath);
        if (pluginDir.cdUp()) {
            QString qmlFromPlugin = pluginDir.absoluteFilePath(QStringLiteral("qml"));
            if (QDir(qmlFromPlugin).exists() && qmlFromPlugin != qtQmlPath) {
                engine->addImportPath(qmlFromPlugin);
                qDebug() << "[QmlMainWindow] Added Qt QML path from plugins:" << qmlFromPlugin;
            }
        }
    }

    // 3. Try common Qt installation paths based on platform
    QStringList commonPaths;

#ifdef Q_OS_WIN
    // Windows: Check common Qt installation locations
    QStringList qtVersions = { QStringLiteral("6.10.1"), QStringLiteral("6.9.0"), QStringLiteral("6.8.0"),
                               QStringLiteral("6.7.0"),  QStringLiteral("6.6.0"), QStringLiteral("6.5.0") };
    QStringList compilers = { QStringLiteral("msvc2022_64"), QStringLiteral("msvc2019_64"), QStringLiteral("mingw_64") };

    for (const QString& ver : qtVersions) {
        for (const QString& compiler : compilers) {
            commonPaths << QStringLiteral("C:/Qt/%1/%2/qml").arg(ver, compiler);
        }
    }
#elif defined(Q_OS_MACOS)
    // macOS: Homebrew and official Qt installations
    commonPaths << QStringLiteral("/opt/homebrew/opt/qt/qml");
    commonPaths << QStringLiteral("/usr/local/opt/qt/qml");
    commonPaths << QStringLiteral("/opt/homebrew/lib/qt6/qml");
    commonPaths << QDir::homePath() + QStringLiteral("/Qt/*/macos/qml");
#else
    // Linux: System and user installations
    commonPaths << QStringLiteral("/usr/lib/qt6/qml");
    commonPaths << QStringLiteral("/usr/lib64/qt6/qml");
    commonPaths << QStringLiteral("/usr/lib/x86_64-linux-gnu/qt6/qml");
    commonPaths << QStringLiteral("/usr/share/qt6/qml");
    commonPaths << QDir::homePath() + QStringLiteral("/Qt/*/gcc_64/qml");
#endif

    for (const QString& path : commonPaths) {
        if (QDir(path).exists()) {
            engine->addImportPath(path);
            qDebug() << "[QmlMainWindow] Added common Qt QML path:" << path;

            // Also add the corresponding bin directory to PATH to ensure DLLs can be loaded
            QDir qmlDir(path);
            if (qmlDir.cdUp()) {
                QString binPath = qmlDir.absoluteFilePath(QStringLiteral("bin"));
                if (QDir(binPath).exists()) {
#ifdef Q_OS_WIN
                    QString pathEnv = qEnvironmentVariable("PATH");
                    if (!pathEnv.contains(binPath)) {
                        pathEnv = binPath + QChar(';') + pathEnv;
                        qputenv("PATH", pathEnv.toUtf8());
                        qDebug() << "[QmlMainWindow] Added Qt bin to PATH from common:" << binPath;
                    }
#else
                    QString ldPath = qEnvironmentVariable("LD_LIBRARY_PATH");
                    // For Linux, we usually want 'lib' not 'bin' for LD_LIBRARY_PATH
                    QString libPath = qmlDir.absoluteFilePath(QStringLiteral("lib"));
                    if (QDir(libPath).exists() && !ldPath.contains(libPath)) {
                        ldPath = libPath + QChar(':') + ldPath;
                        qputenv("LD_LIBRARY_PATH", ldPath.toUtf8());
                        qDebug() << "[QmlMainWindow] Added Qt lib to LD_LIBRARY_PATH from common:" << libPath;
                    }
#endif
                }
            }

            break;  // Found one, that's enough
        }
    }

    // 4. Application directory for bundled QML modules (all platforms)
    QString appQmlPath = QCoreApplication::applicationDirPath() + QStringLiteral("/qml");
    if (QDir(appQmlPath).exists()) {
        engine->addImportPath(appQmlPath);
        qDebug() << "[QmlMainWindow] Added app QML import path:" << appQmlPath;
    }
    
    // 5. Qt Resource System - for qmldir modules in qrc
    engine->addImportPath(QStringLiteral("qrc:/qml"));
    qDebug() << "[QmlMainWindow] Added QRC import path: qrc:/qml";

    // 6. Source directory for development builds (to find ProjTLauncher module)
    // Start from application dir and go up to find source tree
    QDir sourceDir(QCoreApplication::applicationDirPath());
    // First go up from build directory
    if (sourceDir.cdUp()) {
        for (int i = 0; i < 4; ++i) {
            QDir qmlSourceDir(sourceDir);
            if (qmlSourceDir.cd(QStringLiteral("launcher/qml"))) {
                engine->addImportPath(qmlSourceDir.absolutePath());
                qDebug() << "[QmlMainWindow] Added source QML import path:" << qmlSourceDir.absolutePath();
                break;
            }
            if (!sourceDir.cdUp())
                break;
        }
    }

    // 7. Environment variable QML2_IMPORT_PATH (all platforms)
    QString envPath = qEnvironmentVariable("QML2_IMPORT_PATH");
    if (!envPath.isEmpty()) {
#ifdef Q_OS_WIN
        QStringList envPaths = envPath.split(QLatin1Char(';'), Qt::SkipEmptyParts);
#else
        QStringList envPaths = envPath.split(QLatin1Char(':'), Qt::SkipEmptyParts);
#endif
        for (const QString& p : envPaths) {
            if (QDir(p).exists()) {
                engine->addImportPath(p);
                qDebug() << "[QmlMainWindow] Added env QML import path:" << p;
            }
        }
    }

    // Log all current import paths for debugging
    qDebug() << "[QmlMainWindow] Final QML import paths:" << engine->importPathList();
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

QmlMainWindow::QmlMainWindow(LauncherViewModel* launcherViewModel,
                             InstanceListViewModel* instanceListViewModel,
                             NewsViewModel* newsViewModel,
                             SettingsViewModel* settingsViewModel,
                             ThemeViewModel* themeViewModel,
                             QWidget* parent)
    : QMainWindow(parent)
{
    setObjectName(QStringLiteral("QmlMainWindow"));
    setWindowTitle(tr("ProjT Launcher"));
    resize(1000, 700);

    // Ensure the window is deleted when closed so destroyed() is emitted
    // and Application can track open windows correctly.
    setAttribute(Qt::WA_DeleteOnClose, true);

    auto container = new QWidget(this);
    auto layout = new QVBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 0);

    m_quickWidget = new QQuickWidget(container);
    m_quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    m_quickWidget->setClearColor(Qt::transparent);
    
    // Set initial palette for QQuickWidget
    m_quickWidget->setPalette(qApp->palette());

    // Add Qt's QML import paths for development builds loading from source
    QQmlEngine* engine = m_quickWidget->engine();
    setupQmlImportPaths(engine);

    exposeContextProperties(launcherViewModel, instanceListViewModel, newsViewModel, settingsViewModel, themeViewModel,
                            APPLICATION->settings());

    // Update QuickWidget palette when theme changes
    connect(themeViewModel, &ThemeViewModel::themeColorsChanged, this, [this]() {
        if (m_quickWidget) {
            qDebug() << "[QmlMainWindow] Theme changed, updating QuickWidget palette";
            m_quickWidget->setPalette(qApp->palette());
            m_quickWidget->update();
        }
    });

    m_quickWidget->setSource(resolveQmlUrl(QStringLiteral("ShellRoot.qml")));

    layout->addWidget(m_quickWidget);
    setCentralWidget(container);

    m_stateBridge = new ShellStateBridge(APPLICATION->settings(), this);
}

void QmlMainWindow::exposeContextProperties(LauncherViewModel* launcherViewModel,
                                            InstanceListViewModel* instanceListViewModel,
                                            NewsViewModel* newsViewModel,
                                            SettingsViewModel* settingsViewModel,
                                            ThemeViewModel* themeViewModel,
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
    projt->insert(QStringLiteral("themeVM"), QVariant::fromValue(themeViewModel));
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
    if (themeViewModel) {
        ctx->setContextProperty(QStringLiteral("themeVM"), themeViewModel);
        ctx->setContextProperty(QStringLiteral("themeViewModel"), themeViewModel);
    }

    // Create and expose LauncherSettingsViewModel for global launcher settings
    auto launcherSettingsViewModel = new LauncherSettingsViewModel(this);
    ctx->setContextProperty(QStringLiteral("launcherSettingsVM"), launcherSettingsViewModel);
    projt->insert(QStringLiteral("launcherSettingsVM"), QVariant::fromValue(launcherSettingsViewModel));

    // Create and expose AccountsViewModel for account management
    auto accountsViewModel = new AccountsViewModel(this);
    ctx->setContextProperty(QStringLiteral("accountsVM"), accountsViewModel);
    ctx->setContextProperty(QStringLiteral("accountsViewModel"), accountsViewModel);
    projt->insert(QStringLiteral("accountsVM"), QVariant::fromValue(accountsViewModel));

    // Create and expose NewInstanceViewModel for instance creation
    auto newInstanceViewModel = new NewInstanceViewModel(this);
    ctx->setContextProperty(QStringLiteral("newInstanceVM"), newInstanceViewModel);
    ctx->setContextProperty(QStringLiteral("newInstanceViewModel"), newInstanceViewModel);
    projt->insert(QStringLiteral("newInstanceVM"), QVariant::fromValue(newInstanceViewModel));

    // Create and expose InstanceViewModel for selected instance details
    auto instanceViewModel = new InstanceViewModel(this);
    ctx->setContextProperty(QStringLiteral("instanceVM"), instanceViewModel);
    ctx->setContextProperty(QStringLiteral("instanceViewModel"), instanceViewModel);
    projt->insert(QStringLiteral("instanceVM"), QVariant::fromValue(instanceViewModel));

    // Create and expose LogsViewModel for logs page
    auto logsViewModel = new LogsViewModel(this);
    ctx->setContextProperty(QStringLiteral("logsVM"), logsViewModel);
    ctx->setContextProperty(QStringLiteral("logsViewModel"), logsViewModel);
    projt->insert(QStringLiteral("logsVM"), QVariant::fromValue(logsViewModel));

    // Create and expose modplatform ViewModels
    auto atlViewModel = new ATLauncherViewModel(this);
    ctx->setContextProperty(QStringLiteral("atlVM"), atlViewModel);
    projt->insert(QStringLiteral("atlVM"), QVariant::fromValue(atlViewModel));

    auto ftbViewModel = new FTBViewModel(this);
    ctx->setContextProperty(QStringLiteral("ftbVM"), ftbViewModel);
    projt->insert(QStringLiteral("ftbVM"), QVariant::fromValue(ftbViewModel));

    auto technicViewModel = new TechnicViewModel(this);
    ctx->setContextProperty(QStringLiteral("technicVM"), technicViewModel);
    projt->insert(QStringLiteral("technicVM"), QVariant::fromValue(technicViewModel));

    auto curseForgeViewModel = new CurseForgeViewModel(this);
    ctx->setContextProperty(QStringLiteral("curseForgeVM"), curseForgeViewModel);
    projt->insert(QStringLiteral("curseForgeVM"), QVariant::fromValue(curseForgeViewModel));

    auto modrinthViewModel = new ModrinthViewModel(this);
    ctx->setContextProperty(QStringLiteral("modrinthVM"), modrinthViewModel);
    projt->insert(QStringLiteral("modrinthVM"), QVariant::fromValue(modrinthViewModel));

    // Expose TranslationsModel for language selection
    ctx->setContextProperty(QStringLiteral("translationsModel"), APPLICATION->translations().get());

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

void QmlMainWindow::processURLs(const QList<QUrl>& urls)
{
    // TODO: Implement URL processing for importing modpacks
    // This will involve:
    // 1. Parsing URLs to determine type (CurseForge, Modrinth, ATLauncher, etc.)
    // 2. Creating appropriate InstanceImportTask
    // 3. Showing progress dialog
    // 4. Refreshing instance list on completion
    qWarning() << "QmlMainWindow::processURLs() not yet implemented. URLs:" << urls;
}

void QmlMainWindow::closeEvent(QCloseEvent* event)
{
    // When main window is closed, quit the application on all platforms
    // This ensures proper cleanup on Windows, macOS, and Linux
    event->accept();
    QApplication::quit();
}

#include "QmlMainWindow.moc"
