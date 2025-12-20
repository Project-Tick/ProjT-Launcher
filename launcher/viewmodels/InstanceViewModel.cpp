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

#include "InstanceViewModel.h"

#include <algorithm>
#include <QClipboard>
#include <QDateTime>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFileInfoList>
#include <QGuiApplication>
#include <QImage>
#include <QMimeData>
#include <QMessageBox>
#include <QDebug>
#include <QRegularExpression>
#include <QUrl>

#include "Application.h"
#include "FileSystem.h"
#include "InstanceList.h"
#include "MMCZip.h"
#include "minecraft/Component.h"
#include "minecraft/MinecraftInstance.h"
#include "minecraft/PackProfile.h"
#include "minecraft/mod/DataPackFolderModel.h"
#include "minecraft/mod/Mod.h"
#include "minecraft/mod/ModFolderModel.h"
#include "minecraft/mod/Resource.h"
#include "minecraft/mod/ResourcePackFolderModel.h"
#include "minecraft/mod/ShaderPackFolderModel.h"
#include "minecraft/mod/TexturePackFolderModel.h"
#include "ui/dialogs/CustomMessageBox.h"
#include "ui/dialogs/ExportToModListDialog.h"
#include "ui/dialogs/ProgressDialog.h"
#include "ui/dialogs/ResourceDownloadDialog.h"
#include "ui/dialogs/ResourceUpdateDialog.h"

#include "tasks/ConcurrentTask.h"
#include "tasks/Task.h"

// NBT library for servers.dat
#include <io/stream_reader.h>
#include <tag_compound.h>
#include <tag_list.h>
#include <tag_primitive.h>
#include <tag_string.h>
#include <sstream>

namespace {
template <typename TaskPtr>
void runDownloadTasks(QWidget* parent, const QList<TaskPtr>& tasks, const QString& title)
{
    if (tasks.isEmpty()) {
        return;
    }

    auto concurrent = new ConcurrentTask(title, APPLICATION->settings()->get("NumberOfConcurrentDownloads").toInt());
    QObject::connect(concurrent, &Task::failed, [parent, concurrent](const QString& reason) {
        CustomMessageBox::selectable(parent, QObject::tr("Error"), reason, QMessageBox::Critical)->show();
        concurrent->deleteLater();
    });
    QObject::connect(concurrent, &Task::aborted, [parent, concurrent]() {
        CustomMessageBox::selectable(parent, QObject::tr("Aborted"), QObject::tr("Download stopped by user."), QMessageBox::Information)->show();
        concurrent->deleteLater();
    });
    QObject::connect(concurrent, &Task::succeeded, [parent, concurrent]() {
        QStringList warnings = concurrent->warnings();
        if (!warnings.isEmpty()) {
            CustomMessageBox::selectable(parent, QObject::tr("Warnings"), warnings.join('\n'), QMessageBox::Warning)->show();
        }
        concurrent->deleteLater();
    });

    for (const auto& task : tasks) {
        concurrent->addTask(task);
    }

    ProgressDialog progress(parent);
    progress.setSkipButton(true, QObject::tr("Abort"));
    progress.execWithTask(concurrent);
}
}  // namespace

InstanceViewModel::InstanceViewModel(QObject* parent) : QObject(parent) {}

QString InstanceViewModel::instanceId() const
{
    return m_instanceId;
}

void InstanceViewModel::setInstanceId(const QString& id)
{
    if (m_instanceId != id) {
        // Disconnect from old instance
        if (m_instance) {
            disconnect(m_instance.get(), nullptr, this, nullptr);
        }

        m_instanceId = id;

        // Get instance from list
        auto instanceList = APPLICATION->instances();
        if (instanceList) {
            m_instance = instanceList->getInstanceById(id);

            if (m_instance) {
                connect(m_instance.get(), &BaseInstance::propertiesChanged, this, &InstanceViewModel::onInstancePropertiesChanged);
            }
        }

        emit instanceIdChanged();
        emit hasInstanceChanged();
        m_modFilter.clear();
        m_selectedDataPacksPath.clear();
        m_dataPacksFolderModel.reset();
        m_dataPacksModel.clear();
        emit dataPacksModelChanged();
        scanScreenshots();
        scanWorlds();
        scanServers();
        scanMods();
        scanResourcePacks();
        scanShaderPacks();
        scanTexturePacks();
        scanOtherLogs();
        emitAllChanged();
    }
}

bool InstanceViewModel::hasInstance() const
{
    return m_instance != nullptr;
}

QString InstanceViewModel::name() const
{
    return m_instance ? m_instance->name() : QString();
}

void InstanceViewModel::setName(const QString& name)
{
    if (m_instance && m_instance->name() != name) {
        m_instance->setName(name);
        emit nameChanged();
    }
}

QString InstanceViewModel::iconKey() const
{
    return m_instance ? m_instance->iconKey() : QString();
}

void InstanceViewModel::setIconKey(const QString& key)
{
    if (m_instance && m_instance->iconKey() != key) {
        m_instance->setIconKey(key);
        emit iconKeyChanged();
    }
}

QString InstanceViewModel::notes() const
{
    return m_instance ? m_instance->notes() : QString();
}

void InstanceViewModel::setNotes(const QString& notes)
{
    if (m_instance && m_instance->notes() != notes) {
        m_instance->setNotes(notes);
        emit notesChanged();
    }
}

QString InstanceViewModel::instanceType() const
{
    return m_instance ? m_instance->instanceType() : QString();
}

QString InstanceViewModel::instanceRoot() const
{
    return m_instance ? m_instance->instanceRoot() : QString();
}

QString InstanceViewModel::gameRoot() const
{
    return m_instance ? m_instance->gameRoot() : QString();
}

QString InstanceViewModel::instanceLog() const
{
    return m_instanceLog;
}

bool InstanceViewModel::isRunning() const
{
    return m_instance ? m_instance->isRunning() : false;
}

bool InstanceViewModel::hasBrokenVersion() const
{
    return m_instance ? m_instance->hasVersionBroken() : false;
}

bool InstanceViewModel::hasUpdateAvailable() const
{
    return m_instance ? m_instance->hasUpdateAvailable() : false;
}

bool InstanceViewModel::hasCrashed() const
{
    return m_instance ? m_instance->hasCrashed() : false;
}

bool InstanceViewModel::canLaunch() const
{
    return m_instance ? m_instance->canLaunch() : false;
}

bool InstanceViewModel::canEdit() const
{
    return m_instance ? m_instance->canEdit() : false;
}

qint64 InstanceViewModel::totalTimePlayed() const
{
    return m_instance ? m_instance->totalTimePlayed() : 0;
}

qint64 InstanceViewModel::lastTimePlayed() const
{
    return m_instance ? m_instance->lastTimePlayed() : 0;
}

QString InstanceViewModel::totalTimePlayedFormatted() const
{
    return formatTime(totalTimePlayed());
}

QString InstanceViewModel::lastTimePlayedFormatted() const
{
    return formatTime(lastTimePlayed());
}

qint64 InstanceViewModel::lastLaunch() const
{
    return m_instance ? m_instance->lastLaunch() : 0;
}

bool InstanceViewModel::isManagedPack() const
{
    return m_instance ? m_instance->isManagedPack() : false;
}

QString InstanceViewModel::managedPackType() const
{
    return m_instance ? m_instance->getManagedPackType() : QString();
}

QString InstanceViewModel::managedPackName() const
{
    return m_instance ? m_instance->getManagedPackName() : QString();
}

QString InstanceViewModel::managedPackVersionName() const
{
    return m_instance ? m_instance->getManagedPackVersionName() : QString();
}

QString InstanceViewModel::managedPackUrl() const
{
    // Return pack URL based on platform type
    if (!m_instance)
        return QString();

    QString packId = m_instance->getManagedPackID();
    QString platform = managedPackType().toLower();

    if (platform == "modrinth") {
        return QString("https://modrinth.com/modpack/%1").arg(packId);
    } else if (platform == "curseforge" || platform == "flame") {
        return QString("https://www.curseforge.com/minecraft/modpacks/%1").arg(packId);
    }
    return QString();
}

void InstanceViewModel::checkForPackUpdates()
{
    qDebug() << "[InstanceViewModel] Checking for pack updates for:" << m_instanceId;
    // Emit signal to notify QML that update check is in progress
    // The actual update check logic is complex and depends on the pack type (Modrinth/CurseForge)
    // For now, we signal that no update is available - real implementation would need async API calls
    emit packUpdateCheckResult(false, QString());
}

void InstanceViewModel::updatePack()
{
    qDebug() << "[InstanceViewModel] Updating pack for:" << m_instanceId;
    // Pack updates are complex operations that require:
    // 1. Fetching available versions from the API
    // 2. Downloading new pack files
    // 3. Updating instance configuration
    // This should ideally be handled through the existing task system
    // For now, log the request - full implementation would use ManagedPackPage logic
}

void InstanceViewModel::exportPack()
{
    qDebug() << "[InstanceViewModel] Exporting pack for:" << m_instanceId;
    // Emit signal for QML to show export dialog
    emit packExportRequested();
}

void InstanceViewModel::refreshScreenshots()
{
    qDebug() << "[InstanceViewModel] Refreshing screenshots for:" << m_instanceId;
    scanScreenshots();
}

void InstanceViewModel::copyScreenshotToClipboard(int index)
{
    if (index < 0 || index >= m_screenshotPaths.size()) {
        qWarning() << "[InstanceViewModel] Invalid screenshot index:" << index;
        return;
    }

    QString path = m_screenshotPaths.at(index);
    QImage image(path);
    if (!image.isNull()) {
        QClipboard* clipboard = QGuiApplication::clipboard();
        clipboard->setImage(image);
        qDebug() << "[InstanceViewModel] Copied screenshot to clipboard:" << path;
    } else {
        qWarning() << "[InstanceViewModel] Failed to load screenshot:" << path;
    }
}

void InstanceViewModel::deleteScreenshot(int index)
{
    if (index < 0 || index >= m_screenshotPaths.size()) {
        qWarning() << "[InstanceViewModel] Invalid screenshot index:" << index;
        return;
    }

    QString path = m_screenshotPaths.at(index);
    if (QFile::remove(path)) {
        qDebug() << "[InstanceViewModel] Deleted screenshot:" << path;
        scanScreenshots();
    } else {
        qWarning() << "[InstanceViewModel] Failed to delete screenshot:" << path;
    }
}

void InstanceViewModel::openScreenshot(int index)
{
    if (index < 0 || index >= m_screenshotPaths.size()) {
        qWarning() << "[InstanceViewModel] Invalid screenshot index:" << index;
        return;
    }

    QString path = m_screenshotPaths.at(index);
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

void InstanceViewModel::refreshServers()
{
    qDebug() << "[InstanceViewModel] Refreshing servers for:" << m_instanceId;
    scanServers();
}

void InstanceViewModel::addServer(const QString& name, const QString& address)
{
    qDebug() << "[InstanceViewModel] Adding server:" << name << address;

    if (name.isEmpty() || address.isEmpty()) {
        qWarning() << "[InstanceViewModel] Server name and address are required";
        return;
    }

    m_serverNames.append(name);
    m_serverAddresses.append(address);
    saveServers();
    emit serversChanged();
}

void InstanceViewModel::editServer(int index, const QString& name, const QString& address)
{
    qDebug() << "[InstanceViewModel] Editing server" << index << ":" << name << address;

    if (index < 0 || index >= m_serverNames.size()) {
        qWarning() << "[InstanceViewModel] Invalid server index:" << index;
        return;
    }

    m_serverNames[index] = name;
    m_serverAddresses[index] = address;
    saveServers();
    emit serversChanged();
}

void InstanceViewModel::deleteServer(int index)
{
    qDebug() << "[InstanceViewModel] Deleting server" << index;

    if (index < 0 || index >= m_serverNames.size()) {
        qWarning() << "[InstanceViewModel] Invalid server index:" << index;
        return;
    }

    m_serverNames.removeAt(index);
    m_serverAddresses.removeAt(index);
    saveServers();
    emit serversChanged();
}

void InstanceViewModel::moveServerUp(int index)
{
    qDebug() << "[InstanceViewModel] Moving server up:" << index;

    if (index <= 0 || index >= m_serverNames.size()) {
        qWarning() << "[InstanceViewModel] Cannot move server up from index:" << index;
        return;
    }

    m_serverNames.swapItemsAt(index, index - 1);
    m_serverAddresses.swapItemsAt(index, index - 1);
    saveServers();
    emit serversChanged();
}

void InstanceViewModel::moveServerDown(int index)
{
    qDebug() << "[InstanceViewModel] Moving server down:" << index;

    if (index < 0 || index >= m_serverNames.size() - 1) {
        qWarning() << "[InstanceViewModel] Cannot move server down from index:" << index;
        return;
    }

    m_serverNames.swapItemsAt(index, index + 1);
    m_serverAddresses.swapItemsAt(index, index + 1);
    saveServers();
    emit serversChanged();
}

void InstanceViewModel::refreshWorlds()
{
    qDebug() << "[InstanceViewModel] Refreshing worlds for:" << m_instanceId;
    scanWorlds();
}

void InstanceViewModel::importWorld()
{
    qDebug() << "[InstanceViewModel] Requesting world import for:" << m_instanceId;
    // Emit signal for QML to open file dialog
    emit worldImportRequested();
}

void InstanceViewModel::importWorldFromPath(const QString& path)
{
    qDebug() << "[InstanceViewModel] Importing world from:" << path;

    if (path.isEmpty()) {
        qWarning() << "[InstanceViewModel] Empty import path";
        return;
    }

    QFileInfo fileInfo(path);
    if (!fileInfo.exists()) {
        qWarning() << "[InstanceViewModel] Import path does not exist:" << path;
        return;
    }

    QString savesDir = gameRoot() + "/saves";
    QDir().mkpath(savesDir);

    if (fileInfo.isDir()) {
        // Copy world folder
        QString dstPath = savesDir + "/" + fileInfo.fileName();

        // Make unique name if already exists
        int counter = 1;
        while (QDir(dstPath).exists()) {
            dstPath = savesDir + "/" + fileInfo.baseName() + QString(" (%1)").arg(counter++);
        }

        if (FS::copy(path, dstPath)()) {
            qDebug() << "[InstanceViewModel] Imported world to:" << dstPath;
            scanWorlds();
        } else {
            qWarning() << "[InstanceViewModel] Failed to import world";
        }
    } else if (path.endsWith(".zip", Qt::CaseInsensitive)) {
        // Extract zip to saves folder
        QString worldName = fileInfo.baseName();
        QString dstPath = savesDir + "/" + worldName;

        // Make unique name if already exists
        int counter = 1;
        while (QDir(dstPath).exists()) {
            dstPath = savesDir + "/" + worldName + QString(" (%1)").arg(counter++);
        }

        QDir().mkpath(dstPath);
        if (MMCZip::extractDir(path, dstPath).has_value()) {
            qDebug() << "[InstanceViewModel] Extracted world to:" << dstPath;
            scanWorlds();
        } else {
            qWarning() << "[InstanceViewModel] Failed to extract world zip";
            QDir(dstPath).removeRecursively();
        }
    } else {
        qWarning() << "[InstanceViewModel] Unsupported world format:" << path;
    }
}

void InstanceViewModel::copyWorld(int index)
{
    if (index < 0 || index >= m_worldPaths.size()) {
        qWarning() << "[InstanceViewModel] Invalid world index:" << index;
        return;
    }

    QString srcPath = m_worldPaths.at(index);
    QString srcName = m_worldNames.at(index);
    QString dstName = srcName + " - Copy";
    QString dstPath = QFileInfo(srcPath).absolutePath() + "/" + dstName;

    // Make unique name
    int counter = 1;
    while (QDir(dstPath).exists()) {
        dstPath = QFileInfo(srcPath).absolutePath() + "/" + srcName + QString(" - Copy %1").arg(counter++);
    }

    if (FS::copy(srcPath, dstPath)()) {
        qDebug() << "[InstanceViewModel] Copied world to:" << dstPath;
        scanWorlds();
    } else {
        qWarning() << "[InstanceViewModel] Failed to copy world";
    }
}

void InstanceViewModel::backupWorld(int index)
{
    if (index < 0 || index >= m_worldPaths.size()) {
        qWarning() << "[InstanceViewModel] Invalid world index:" << index;
        return;
    }

    QString worldPath = m_worldPaths.at(index);
    QString worldName = m_worldNames.at(index);

    // Create backup in instance folder with timestamp
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss");
    QString backupName = QString("%1_backup_%2.zip").arg(worldName, timestamp);
    QString backupDir = gameRoot() + "/world_backups";
    QString backupPath = backupDir + "/" + backupName;

    // Ensure backup directory exists
    QDir().mkpath(backupDir);

    // Create zip backup using JlCompress (from quazip)
    bool success = JlCompress::compressDir(backupPath, worldPath);
    if (success) {
        qDebug() << "[InstanceViewModel] Created world backup:" << backupPath;
        emit worldBackupCompleted(true, backupPath);
    } else {
        qWarning() << "[InstanceViewModel] Failed to create world backup";
        emit worldBackupCompleted(false, QString());
    }
}

void InstanceViewModel::deleteWorld(int index)
{
    if (index < 0 || index >= m_worldPaths.size()) {
        qWarning() << "[InstanceViewModel] Invalid world index:" << index;
        return;
    }

    QString path = m_worldPaths.at(index);
    if (FS::deletePath(path)) {
        qDebug() << "[InstanceViewModel] Deleted world:" << path;
        scanWorlds();
    } else {
        qWarning() << "[InstanceViewModel] Failed to delete world:" << path;
    }
}

void InstanceViewModel::openWorldFolder(int index)
{
    if (index < 0 || index >= m_worldPaths.size()) {
        qWarning() << "[InstanceViewModel] Invalid world index:" << index;
        return;
    }

    QString path = m_worldPaths.at(index);
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

bool InstanceViewModel::overrideJava() const
{
    if (!m_instance)
        return false;
    auto settings = m_instance->settings();
    return settings ? settings->get("OverrideJavaLocation").toBool() : false;
}

void InstanceViewModel::setOverrideJava(bool override)
{
    if (!m_instance)
        return;
    auto settings = m_instance->settings();
    if (settings) {
        settings->set("OverrideJavaLocation", override);
        emit overrideJavaChanged();
    }
}

QString InstanceViewModel::javaPath() const
{
    if (!m_instance)
        return QString();
    auto settings = m_instance->settings();
    return settings ? settings->get("JavaPath").toString() : QString();
}

void InstanceViewModel::setJavaPath(const QString& path)
{
    if (!m_instance)
        return;
    auto settings = m_instance->settings();
    if (settings) {
        settings->set("JavaPath", path);
        emit javaPathChanged();
    }
}

QString InstanceViewModel::jvmArgs() const
{
    if (!m_instance)
        return QString();
    auto settings = m_instance->settings();
    return settings ? settings->get("JvmArgs").toString() : QString();
}

void InstanceViewModel::setJvmArgs(const QString& args)
{
    if (!m_instance)
        return;
    auto settings = m_instance->settings();
    if (settings) {
        settings->set("JvmArgs", args);
        emit jvmArgsChanged();
    }
}

void InstanceViewModel::autoDetectJava(const QString& instanceId)
{
    Q_UNUSED(instanceId)
    // Search for Java installations in common paths
    QStringList javaPaths;

#ifdef Q_OS_WIN
    javaPaths << "C:/Program Files/Java"
              << "C:/Program Files (x86)/Java"
              << "C:/Program Files/Eclipse Adoptium"
              << "C:/Program Files/AdoptOpenJDK" << QDir::homePath() + "/.jdks";
#elif defined(Q_OS_MAC)
    javaPaths << "/Library/Java/JavaVirtualMachines"
              << "/System/Library/Frameworks/JavaVM.framework/Versions" << QDir::homePath() + "/Library/Java/JavaVirtualMachines";
#else
    javaPaths << "/usr/lib/jvm"
              << "/usr/lib64/jvm"
              << "/usr/local/lib/jvm" << QDir::homePath() + "/.jdks" << QDir::homePath() + "/.sdkman/candidates/java";
#endif

    QStringList foundJavas;
    for (const QString& basePath : javaPaths) {
        QDir dir(basePath);
        if (!dir.exists())
            continue;

        QStringList jdkDirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (const QString& jdkDir : jdkDirs) {
            QString javaExe = basePath + "/" + jdkDir + "/bin/java";
#ifdef Q_OS_WIN
            javaExe += ".exe";
#endif
            if (QFile::exists(javaExe)) {
                foundJavas << javaExe;
            }
        }
    }

    // Also check PATH
    QString pathEnv = qgetenv("PATH");
#ifdef Q_OS_WIN
    QStringList pathDirs = pathEnv.split(';');
#else
    QStringList pathDirs = pathEnv.split(':');
#endif
    for (const QString& pathDir : pathDirs) {
        QString javaExe = pathDir + "/java";
#ifdef Q_OS_WIN
        javaExe += ".exe";
#endif
        if (QFile::exists(javaExe) && !foundJavas.contains(javaExe)) {
            foundJavas.prepend(javaExe);
        }
    }

    emit javaAutoDetected(foundJavas);
}

bool InstanceViewModel::overrideMemory() const
{
    if (!m_instance)
        return false;
    auto settings = m_instance->settings();
    return settings ? settings->get("OverrideMemory").toBool() : false;
}

void InstanceViewModel::setOverrideMemory(bool override)
{
    if (!m_instance)
        return;
    auto settings = m_instance->settings();
    if (settings) {
        settings->set("OverrideMemory", override);
        emit overrideMemoryChanged();
    }
}

int InstanceViewModel::minMemory() const
{
    if (!m_instance)
        return 512;
    auto settings = m_instance->settings();
    return settings ? settings->get("MinMemAlloc").toInt() : 512;
}

void InstanceViewModel::setMinMemory(int mb)
{
    if (!m_instance)
        return;
    auto settings = m_instance->settings();
    if (settings) {
        settings->set("MinMemAlloc", mb);
        emit minMemoryChanged();
    }
}

int InstanceViewModel::maxMemory() const
{
    if (!m_instance)
        return 4096;
    auto settings = m_instance->settings();
    return settings ? settings->get("MaxMemAlloc").toInt() : 4096;
}

void InstanceViewModel::setMaxMemory(int mb)
{
    if (!m_instance)
        return;
    auto settings = m_instance->settings();
    if (settings) {
        settings->set("MaxMemAlloc", mb);
        emit maxMemoryChanged();
    }
}

bool InstanceViewModel::overrideWindow() const
{
    if (!m_instance)
        return false;
    auto settings = m_instance->settings();
    return settings ? settings->get("OverrideWindow").toBool() : false;
}

void InstanceViewModel::setOverrideWindow(bool override)
{
    if (!m_instance)
        return;
    auto settings = m_instance->settings();
    if (settings) {
        settings->set("OverrideWindow", override);
        emit overrideWindowChanged();
    }
}

int InstanceViewModel::windowWidth() const
{
    if (!m_instance)
        return 854;
    auto settings = m_instance->settings();
    return settings ? settings->get("MinecraftWinWidth").toInt() : 854;
}

void InstanceViewModel::setWindowWidth(int width)
{
    if (!m_instance)
        return;
    auto settings = m_instance->settings();
    if (settings) {
        settings->set("MinecraftWinWidth", width);
        emit windowWidthChanged();
    }
}

int InstanceViewModel::windowHeight() const
{
    if (!m_instance)
        return 480;
    auto settings = m_instance->settings();
    return settings ? settings->get("MinecraftWinHeight").toInt() : 480;
}

void InstanceViewModel::setWindowHeight(int height)
{
    if (!m_instance)
        return;
    auto settings = m_instance->settings();
    if (settings) {
        settings->set("MinecraftWinHeight", height);
        emit windowHeightChanged();
    }
}

bool InstanceViewModel::maximizeWindow() const
{
    if (!m_instance)
        return false;
    auto settings = m_instance->settings();
    return settings ? settings->get("LaunchMaximized").toBool() : false;
}

void InstanceViewModel::setMaximizeWindow(bool maximize)
{
    if (!m_instance)
        return;
    auto settings = m_instance->settings();
    if (settings) {
        settings->set("LaunchMaximized", maximize);
        emit maximizeWindowChanged();
    }
}

bool InstanceViewModel::fullscreen() const
{
    if (!m_instance)
        return false;
    auto settings = m_instance->settings();
    return settings ? settings->get("LaunchFullscreen").toBool() : false;
}

void InstanceViewModel::setFullscreen(bool fs)
{
    if (!m_instance)
        return;
    auto settings = m_instance->settings();
    if (settings) {
        settings->set("LaunchFullscreen", fs);
        emit fullscreenChanged();
    }
}

bool InstanceViewModel::showConsole() const
{
    if (!m_instance)
        return true;
    auto settings = m_instance->settings();
    return settings ? settings->get("ShowConsole").toBool() : true;
}

void InstanceViewModel::setShowConsole(bool show)
{
    if (!m_instance)
        return;
    auto settings = m_instance->settings();
    if (settings) {
        settings->set("ShowConsole", show);
        emit showConsoleChanged();
    }
}

bool InstanceViewModel::closeOnLaunch() const
{
    if (!m_instance)
        return false;
    auto settings = m_instance->settings();
    return settings ? settings->get("CloseAfterLaunch").toBool() : false;
}

void InstanceViewModel::setCloseOnLaunch(bool close)
{
    if (!m_instance)
        return;
    auto settings = m_instance->settings();
    if (settings) {
        settings->set("CloseAfterLaunch", close);
        emit closeOnLaunchChanged();
    }
}

bool InstanceViewModel::quitAfterGame() const
{
    if (!m_instance)
        return false;
    auto settings = m_instance->settings();
    return settings ? settings->get("QuitAfterGameStop").toBool() : false;
}

void InstanceViewModel::setQuitAfterGame(bool quit)
{
    if (!m_instance)
        return;
    auto settings = m_instance->settings();
    if (settings) {
        settings->set("QuitAfterGameStop", quit);
        emit quitAfterGameChanged();
    }
}

QString InstanceViewModel::preLaunchCommand() const
{
    return m_instance ? m_instance->getPreLaunchCommand() : QString();
}

void InstanceViewModel::setPreLaunchCommand(const QString& cmd)
{
    if (!m_instance)
        return;
    auto settings = m_instance->settings();
    if (settings) {
        settings->set("PreLaunchCommand", cmd);
        emit preLaunchCommandChanged();
    }
}

QString InstanceViewModel::postExitCommand() const
{
    return m_instance ? m_instance->getPostExitCommand() : QString();
}

void InstanceViewModel::setPostExitCommand(const QString& cmd)
{
    if (!m_instance)
        return;
    auto settings = m_instance->settings();
    if (settings) {
        settings->set("PostExitCommand", cmd);
        emit postExitCommandChanged();
    }
}

QString InstanceViewModel::wrapperCommand() const
{
    return m_instance ? m_instance->getWrapperCommand() : QString();
}

void InstanceViewModel::setWrapperCommand(const QString& cmd)
{
    if (!m_instance)
        return;
    auto settings = m_instance->settings();
    if (settings) {
        settings->set("WrapperCommand", cmd);
        emit wrapperCommandChanged();
    }
}

void InstanceViewModel::launch()
{
    if (m_instance && m_instance->canLaunch()) {
        emit launchRequested(m_instanceId);
    }
}

void InstanceViewModel::launchOffline()
{
    if (m_instance && m_instance->canLaunch()) {
        emit launchOfflineRequested(m_instanceId);
    }
}

void InstanceViewModel::kill()
{
    if (m_instance && m_instance->isRunning()) {
        emit killRequested(m_instanceId);
    }
}

void InstanceViewModel::refreshInstanceLog()
{
    loadLatestLog();
}

void InstanceViewModel::copyLogToClipboard()
{
    if (auto clipboard = QGuiApplication::clipboard()) {
        clipboard->setText(m_instanceLog);
    }
}

void InstanceViewModel::uploadLog()
{
    // TODO: integrate with paste service. For now, copy to clipboard so the user can share manually.
    copyLogToClipboard();
}

void InstanceViewModel::clearLog()
{
    if (!m_instanceLog.isEmpty()) {
        m_instanceLog.clear();
        emit instanceLogChanged();
    }
}

void InstanceViewModel::findInLog(const QString& text)
{
    if (text.isEmpty()) {
        return;
    }

    const int index = m_instanceLog.indexOf(text, 0, Qt::CaseInsensitive);
    if (index >= 0) {
        qDebug() << "[InstanceViewModel] Found text in log at index" << index << "for instance" << m_instanceId;
    } else {
        qDebug() << "[InstanceViewModel] Text not found in log for instance" << m_instanceId;
    }
}

void InstanceViewModel::openFolder()
{
    if (m_instance) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(m_instance->instanceRoot()));
    }
}

void InstanceViewModel::openGameFolder()
{
    if (m_instance) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(m_instance->gameRoot()));
    }
}

void InstanceViewModel::openModsFolder()
{
    if (m_instance) {
        QString modsPath = m_instance->modsRoot();
        QDir dir(modsPath);
        if (!dir.exists()) {
            dir.mkpath(".");
        }
        QDesktopServices::openUrl(QUrl::fromLocalFile(modsPath));
    }
}

void InstanceViewModel::openConfigsFolder()
{
    if (!m_instance)
        return;

    auto mcInstance = std::dynamic_pointer_cast<MinecraftInstance>(m_instance);
    if (!mcInstance)
        return;

    QString path = mcInstance->instanceConfigFolder();
    QDir dir(path);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

void InstanceViewModel::openResourcePacksFolder()
{
    if (m_instance) {
        QString path = m_instance->gameRoot() + "/resourcepacks";
        QDir dir(path);
        if (!dir.exists()) {
            dir.mkpath(".");
        }
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    }
}

void InstanceViewModel::openShaderPacksFolder()
{
    if (m_instance) {
        QString path = m_instance->gameRoot() + "/shaderpacks";
        QDir dir(path);
        if (!dir.exists()) {
            dir.mkpath(".");
        }
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    }
}

void InstanceViewModel::openScreenshotsFolder()
{
    if (m_instance) {
        QString path = m_instance->gameRoot() + "/screenshots";
        QDir dir(path);
        if (!dir.exists()) {
            dir.mkpath(".");
        }
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    }
}

void InstanceViewModel::saveSettings()
{
    if (m_instance) {
        m_instance->saveNow();
    }
}

void InstanceViewModel::reloadSettings()
{
    if (m_instance) {
        m_instance->reloadSettings();
        emitAllChanged();
    }
}

void InstanceViewModel::resetTimePlayed()
{
    if (m_instance) {
        m_instance->resetTimePlayed();
        emit totalTimePlayedChanged();
        emit lastTimePlayedChanged();
    }
}

QVariantMap InstanceViewModel::getInstanceInfo() const
{
    QVariantMap info;

    if (!m_instance) {
        return info;
    }

    info["id"] = m_instanceId;
    info["name"] = name();
    info["iconKey"] = iconKey();
    info["notes"] = notes();
    info["instanceType"] = instanceType();
    info["instanceRoot"] = instanceRoot();
    info["gameRoot"] = gameRoot();
    info["isRunning"] = isRunning();
    info["hasBrokenVersion"] = hasBrokenVersion();
    info["hasUpdateAvailable"] = hasUpdateAvailable();
    info["hasCrashed"] = hasCrashed();
    info["canLaunch"] = canLaunch();
    info["canEdit"] = canEdit();
    info["totalTimePlayed"] = totalTimePlayed();
    info["totalTimePlayedFormatted"] = totalTimePlayedFormatted();
    info["lastTimePlayed"] = lastTimePlayed();
    info["lastTimePlayedFormatted"] = lastTimePlayedFormatted();
    info["lastLaunch"] = lastLaunch();
    info["isManagedPack"] = isManagedPack();
    info["managedPackType"] = managedPackType();
    info["managedPackName"] = managedPackName();
    info["managedPackVersionName"] = managedPackVersionName();

    return info;
}

QString InstanceViewModel::formatPlayTime(qint64 seconds) const
{
    return formatTime(seconds);
}

void InstanceViewModel::onInstancePropertiesChanged(BaseInstance* inst)
{
    if (inst == m_instance.get()) {
        emitAllChanged();
    }
}

void InstanceViewModel::loadLatestLog()
{
    m_currentLogPath.clear();

    QString content;
    if (!m_instance) {
        if (m_instanceLog != content) {
            m_instanceLog = content;
            emit instanceLogChanged();
        }
        return;
    }

    QFileInfo latestFile;
    const QStringList searchPaths = m_instance->getLogFileSearchPaths();
    for (const auto& dirPath : searchPaths) {
        QDir dir(dirPath);
        if (!dir.exists()) {
            continue;
        }

        const QFileInfoList entries =
            dir.entryInfoList(QStringList() << "*.log"
                                            << "*.txt"
                                            << "*.out",
                              QDir::Files, QDir::Time);
        if (!entries.isEmpty()) {
            const QFileInfo& candidate = entries.first();
            if (!latestFile.exists() || candidate.lastModified() > latestFile.lastModified()) {
                latestFile = candidate;
            }
        }
    }

    if (latestFile.exists()) {
        QFile file(latestFile.filePath());
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            constexpr qint64 kMaxBytes = 1024 * 1024;  // read last 1MB to avoid huge loads
            if (file.size() > kMaxBytes) {
                file.seek(file.size() - kMaxBytes);
            }
            content = QString::fromUtf8(file.readAll());
            m_currentLogPath = latestFile.filePath();
        }
    }

    if (m_instanceLog != content) {
        m_instanceLog = content;
        emit instanceLogChanged();
    }
}

void InstanceViewModel::loadFromInstance()
{
    emitAllChanged();
}

void InstanceViewModel::emitAllChanged()
{
    loadLatestLog();
    emit nameChanged();
    emit iconKeyChanged();
    emit notesChanged();
    emit instanceTypeChanged();
    emit instanceRootChanged();
    emit gameRootChanged();
    emit isRunningChanged();
    emit hasBrokenVersionChanged();
    emit hasUpdateAvailableChanged();
    emit hasCrashedChanged();
    emit canLaunchChanged();
    emit canEditChanged();
    emit totalTimePlayedChanged();
    emit lastTimePlayedChanged();
    emit lastLaunchChanged();
    emit isManagedPackChanged();
    emit managedPackTypeChanged();
    emit managedPackNameChanged();
    emit managedPackVersionNameChanged();
    emit managedPackUrlChanged();
    emit overrideJavaChanged();
    emit javaPathChanged();
    emit jvmArgsChanged();
    emit overrideMemoryChanged();
    emit minMemoryChanged();
    emit maxMemoryChanged();
    emit overrideWindowChanged();
    emit windowWidthChanged();
    emit windowHeightChanged();
    emit maximizeWindowChanged();
    emit fullscreenChanged();
    emit showConsoleChanged();
    emit closeOnLaunchChanged();
    emit quitAfterGameChanged();
    emit preLaunchCommandChanged();
    emit postExitCommandChanged();
    emit wrapperCommandChanged();
}

QString InstanceViewModel::formatTime(qint64 seconds) const
{
    if (seconds <= 0) {
        return tr("Never played");
    }

    qint64 hours = seconds / 3600;
    qint64 minutes = (seconds % 3600) / 60;

    if (hours > 0) {
        return tr("%1h %2m").arg(hours).arg(minutes);
    } else if (minutes > 0) {
        return tr("%1 minutes").arg(minutes);
    } else {
        return tr("Less than a minute");
    }
}

QStringList InstanceViewModel::screenshotPaths() const
{
    return m_screenshotPaths;
}

QStringList InstanceViewModel::screenshotNames() const
{
    return m_screenshotNames;
}

QStringList InstanceViewModel::worldPaths() const
{
    return m_worldPaths;
}

QStringList InstanceViewModel::worldNames() const
{
    return m_worldNames;
}

void InstanceViewModel::scanScreenshots()
{
    m_screenshotPaths.clear();
    m_screenshotNames.clear();

    if (!m_instance) {
        emit screenshotPathsChanged();
        return;
    }

    QString screenshotsPath = gameRoot() + "/screenshots";
    QDir dir(screenshotsPath);

    if (dir.exists()) {
        QStringList filters;
        filters << "*.png"
                << "*.jpg"
                << "*.jpeg";
        dir.setNameFilters(filters);
        dir.setSorting(QDir::Time | QDir::Reversed);

        QFileInfoList files = dir.entryInfoList(QDir::Files);
        for (const QFileInfo& file : files) {
            m_screenshotPaths.append(file.absoluteFilePath());
            m_screenshotNames.append(file.fileName());
        }
    }

    emit screenshotPathsChanged();
    qDebug() << "[InstanceViewModel] Found" << m_screenshotPaths.size() << "screenshots";
}

void InstanceViewModel::scanWorlds()
{
    m_worldPaths.clear();
    m_worldNames.clear();

    if (!m_instance) {
        emit worldPathsChanged();
        return;
    }

    QString savesPath = gameRoot() + "/saves";
    QDir dir(savesPath);

    if (dir.exists()) {
        QFileInfoList entries = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (const QFileInfo& entry : entries) {
            // Check if it's a valid world folder (has level.dat)
            if (QFile::exists(entry.absoluteFilePath() + "/level.dat")) {
                m_worldPaths.append(entry.absoluteFilePath());
                m_worldNames.append(entry.fileName());
            }
        }
    }

    emit worldPathsChanged();
    qDebug() << "[InstanceViewModel] Found" << m_worldPaths.size() << "worlds";
}

// ============ Servers ============

QStringList InstanceViewModel::serverNames() const
{
    return m_serverNames;
}

QStringList InstanceViewModel::serverAddresses() const
{
    return m_serverAddresses;
}

void InstanceViewModel::scanServers()
{
    m_serverNames.clear();
    m_serverAddresses.clear();

    if (!m_instance) {
        emit serversChanged();
        return;
    }

    QString serversPath = gameRoot() + "/servers.dat";

    if (!QFile::exists(serversPath)) {
        emit serversChanged();
        qDebug() << "[InstanceViewModel] No servers.dat found";
        return;
    }

    try {
        QByteArray input = FS::read(serversPath);
        std::istringstream stream(std::string(input.constData(), input.size()));
        auto pair = nbt::io::read_compound(stream);

        if (pair.first != "" || pair.second == nullptr) {
            qWarning() << "[InstanceViewModel] Invalid servers.dat format";
            emit serversChanged();
            return;
        }

        auto& root = *pair.second;
        if (!root.has_key("servers", nbt::tag_type::List)) {
            emit serversChanged();
            return;
        }

        auto& serversList = root["servers"].as<nbt::tag_list>();
        for (auto& serverTag : serversList) {
            auto& server = serverTag.as<nbt::tag_compound>();

            std::string nameStr(server["name"]);
            std::string addressStr(server["ip"]);

            m_serverNames.append(QString::fromUtf8(nameStr.c_str()));
            m_serverAddresses.append(QString::fromUtf8(addressStr.c_str()));
        }

        qDebug() << "[InstanceViewModel] Found" << m_serverNames.size() << "servers";
    } catch (const std::exception& e) {
        qWarning() << "[InstanceViewModel] Failed to read servers.dat:" << e.what();
    }

    emit serversChanged();
}

void InstanceViewModel::saveServers()
{
    if (!m_instance) {
        return;
    }

    QString serversPath = gameRoot() + "/servers.dat";

    try {
        nbt::tag_compound root;
        nbt::tag_list serversList(nbt::tag_type::Compound);

        for (int i = 0; i < m_serverNames.size(); i++) {
            nbt::tag_compound server;
            server.insert("name", m_serverNames[i].toUtf8().toStdString());
            server.insert("ip", m_serverAddresses[i].toUtf8().toStdString());
            serversList.push_back(std::move(server));
        }

        root.insert("servers", std::move(serversList));

        // Ensure directory exists
        if (!FS::ensureFilePathExists(serversPath)) {
            qWarning() << "[InstanceViewModel] Failed to create path for servers.dat";
            return;
        }

        std::ostringstream stream;
        nbt::io::write_tag("", root, stream);
        QByteArray data(stream.str().data(), (int)stream.str().size());
        FS::write(serversPath, data);

        qDebug() << "[InstanceViewModel] Saved" << m_serverNames.size() << "servers";
    } catch (const std::exception& e) {
        qWarning() << "[InstanceViewModel] Failed to write servers.dat:" << e.what();
    }
}

// ============ Version Info ============

QString InstanceViewModel::minecraftVersion() const
{
    if (!m_instance)
        return QString();

    auto mcInstance = std::dynamic_pointer_cast<MinecraftInstance>(m_instance);
    if (!mcInstance)
        return QString();

    auto profile = mcInstance->getPackProfile();
    if (!profile)
        return QString();

    return profile->getComponentVersion("net.minecraft");
}

QString InstanceViewModel::modLoaderName() const
{
    if (!m_instance)
        return QString();

    auto mcInstance = std::dynamic_pointer_cast<MinecraftInstance>(m_instance);
    if (!mcInstance)
        return QString();

    auto profile = mcInstance->getPackProfile();
    if (!profile)
        return QString();

    // Check for various mod loaders
    if (!profile->getComponentVersion("net.minecraftforge").isEmpty())
        return "Forge";
    if (!profile->getComponentVersion("net.neoforged").isEmpty())
        return "NeoForge";
    if (!profile->getComponentVersion("net.fabricmc.fabric-loader").isEmpty())
        return "Fabric";
    if (!profile->getComponentVersion("org.quiltmc.quilt-loader").isEmpty())
        return "Quilt";

    return QString();
}

QString InstanceViewModel::modLoaderVersion() const
{
    if (!m_instance)
        return QString();

    auto mcInstance = std::dynamic_pointer_cast<MinecraftInstance>(m_instance);
    if (!mcInstance)
        return QString();

    auto profile = mcInstance->getPackProfile();
    if (!profile)
        return QString();

    // Check for various mod loaders
    QString version = profile->getComponentVersion("net.minecraftforge");
    if (!version.isEmpty())
        return version;

    version = profile->getComponentVersion("net.neoforged");
    if (!version.isEmpty())
        return version;

    version = profile->getComponentVersion("net.fabricmc.fabric-loader");
    if (!version.isEmpty())
        return version;

    version = profile->getComponentVersion("org.quiltmc.quilt-loader");
    if (!version.isEmpty())
        return version;

    return QString();
}

QVariantList InstanceViewModel::componentsModel() const
{
    QVariantList result;

    if (!m_instance)
        return result;

    auto mcInstance = std::dynamic_pointer_cast<MinecraftInstance>(m_instance);
    if (!mcInstance)
        return result;

    auto profile = mcInstance->getPackProfile();
    if (!profile)
        return result;

    for (int i = 0; i < profile->rowCount(); i++) {
        auto component = profile->getComponent(i);
        if (!component)
            continue;

        QVariantMap item;
        item["name"] = component->getName();
        item["version"] = component->getVersion();
        item["uid"] = component->getID();
        item["required"] = !component->isCustomizable();
        item["enabled"] = component->isEnabled();
        item["canToggle"] = component->canBeDisabled();
        item["canRemove"] = component->isRemovable();
        item["canMoveUp"] = i > 0 && component->isMoveable();
        item["canMoveDown"] = i < profile->rowCount() - 1 && component->isMoveable();

        result.append(item);
    }

    return result;
}

void InstanceViewModel::refreshVersionComponents()
{
    emit componentsModelChanged();
    emit versionChanged();
}

void InstanceViewModel::setComponentEnabled(int index, bool enabled)
{
    if (!m_instance)
        return;

    auto mcInstance = std::dynamic_pointer_cast<MinecraftInstance>(m_instance);
    if (!mcInstance)
        return;

    auto profile = mcInstance->getPackProfile();
    if (!profile || index < 0 || index >= profile->rowCount())
        return;

    auto component = profile->getComponent(index);
    if (component && component->canBeDisabled()) {
        component->setEnabled(enabled);
        emit componentsModelChanged();
    }
}

void InstanceViewModel::moveComponentUp(int index)
{
    if (!m_instance)
        return;

    auto mcInstance = std::dynamic_pointer_cast<MinecraftInstance>(m_instance);
    if (!mcInstance)
        return;

    auto profile = mcInstance->getPackProfile();
    if (!profile || index <= 0 || index >= profile->rowCount())
        return;

    profile->move(index, PackProfile::MoveUp);
    emit componentsModelChanged();
}

void InstanceViewModel::moveComponentDown(int index)
{
    if (!m_instance)
        return;

    auto mcInstance = std::dynamic_pointer_cast<MinecraftInstance>(m_instance);
    if (!mcInstance)
        return;

    auto profile = mcInstance->getPackProfile();
    if (!profile || index < 0 || index >= profile->rowCount() - 1)
        return;

    profile->move(index, PackProfile::MoveDown);
    emit componentsModelChanged();
}

void InstanceViewModel::removeComponent(int index)
{
    if (!m_instance)
        return;

    auto mcInstance = std::dynamic_pointer_cast<MinecraftInstance>(m_instance);
    if (!mcInstance)
        return;

    auto profile = mcInstance->getPackProfile();
    if (!profile || index < 0 || index >= profile->rowCount())
        return;

    auto component = profile->getComponent(index);
    if (component && component->isRemovable()) {
        profile->remove(index);
        emit componentsModelChanged();
        emit versionChanged();
    }
}

void InstanceViewModel::changeMinecraftVersion(const QString& version)
{
    if (!m_instance || version.isEmpty())
        return;

    auto mcInstance = std::dynamic_pointer_cast<MinecraftInstance>(m_instance);
    if (!mcInstance)
        return;

    auto profile = mcInstance->getPackProfile();
    if (!profile)
        return;

    profile->setComponentVersion("net.minecraft", version, true);
    emit versionChanged();
    emit componentsModelChanged();
}

void InstanceViewModel::installModLoader(const QString& loaderType, const QString& version)
{
    if (!m_instance || loaderType.isEmpty())
        return;

    auto mcInstance = std::dynamic_pointer_cast<MinecraftInstance>(m_instance);
    if (!mcInstance)
        return;

    auto profile = mcInstance->getPackProfile();
    if (!profile)
        return;

    QString uid;
    if (loaderType.toLower() == "forge")
        uid = "net.minecraftforge";
    else if (loaderType.toLower() == "neoforge")
        uid = "net.neoforged";
    else if (loaderType.toLower() == "fabric")
        uid = "net.fabricmc.fabric-loader";
    else if (loaderType.toLower() == "quilt")
        uid = "org.quiltmc.quilt-loader";

    if (!uid.isEmpty()) {
        if (version.isEmpty()) {
            profile->installEmpty(uid, loaderType);
        } else {
            profile->setComponentVersion(uid, version, true);
        }
        emit versionChanged();
        emit componentsModelChanged();
    }
}

// ============ Mods ============

QVariantList InstanceViewModel::modsModel() const
{
    return m_modsModel;
}

int InstanceViewModel::modsCount() const
{
    return m_modsModel.size();
}

void InstanceViewModel::refreshMods()
{
    scanMods();
}

void InstanceViewModel::addMod(const QString& filePath)
{
    if (!m_instance || filePath.isEmpty())
        return;

    auto mcInstance = std::dynamic_pointer_cast<MinecraftInstance>(m_instance);
    if (!mcInstance)
        return;

    auto modsModel = mcInstance->loaderModList();
    if (modsModel) {
        modsModel->installResource(filePath);
        modsModel->update();
        scanMods();
    }
}

void InstanceViewModel::browseForMods()
{
    if (!m_instance)
        return;

    QString startDir = m_instance->modsRoot();
    QStringList files =
        QFileDialog::getOpenFileNames(nullptr, tr("Select Mod Files"), startDir, tr("Mod Files (*.jar *.zip);;All Files (*)"));
    for (const auto& path : files) {
        addMod(path);
    }
}

void InstanceViewModel::filterMods(const QString& text)
{
    m_modFilter = text;
    scanMods();
}

void InstanceViewModel::removeMod(int index)
{
    if (!m_instance || index < 0 || index >= m_modsModel.size())
        return;

    auto mcInstance = std::dynamic_pointer_cast<MinecraftInstance>(m_instance);
    if (!mcInstance)
        return;

    auto modsModel = mcInstance->loaderModList();
    if (modsModel) {
        modsModel->deleteResources(QModelIndexList() << modsModel->index(index, 0));
        scanMods();
    }
}

void InstanceViewModel::enableMod(int index, bool enabled)
{
    if (!m_instance || index < 0 || index >= m_modsModel.size())
        return;

    auto mcInstance = std::dynamic_pointer_cast<MinecraftInstance>(m_instance);
    if (!mcInstance)
        return;

    auto modsModel = mcInstance->loaderModList();
    if (modsModel) {
        modsModel->setResourceEnabled(QModelIndexList() << modsModel->index(index, 0),
                                      enabled ? EnableAction::ENABLE : EnableAction::DISABLE);
        scanMods();
    }
}

void InstanceViewModel::checkAllModUpdates()
{
    if (!m_instance)
        return;

    auto mcInstance = std::dynamic_pointer_cast<MinecraftInstance>(m_instance);
    if (!mcInstance)
        return;

    auto profile = mcInstance->getPackProfile();
    if (!profile || !profile->getModLoaders().has_value()) {
        QMessageBox::critical(nullptr, tr("Error"), tr("Please install a mod loader first!"));
        return;
    }
    if (APPLICATION->settings()->get("ModMetadataDisabled").toBool()) {
        QMessageBox::critical(nullptr, tr("Error"), tr("Mod updates are unavailable when metadata is disabled!"));
        return;
    }
    if (m_instance->isRunning()) {
        auto response = CustomMessageBox::selectable(nullptr, tr("Confirm Update"),
                                                     tr("Updating mods while the game is running may cause mod duplication and game crashes.\n"
                                                        "The old files may not be deleted as they are in use.\n"
                                                        "Are you sure you want to do this?"),
                                                     QMessageBox::Warning, QMessageBox::Yes | QMessageBox::No, QMessageBox::No)
                            ->exec();
        if (response != QMessageBox::Yes)
            return;
    }

    auto modsModel = mcInstance->loaderModList();
    if (!modsModel)
        return;

    auto modsList = modsModel->allResources();
    if (modsList.isEmpty())
        return;

    ResourceUpdateDialog updateDialog(nullptr, m_instance.get(), modsModel, modsList, false, profile->getModLoadersList());
    updateDialog.checkCandidates();

    if (updateDialog.aborted()) {
        CustomMessageBox::selectable(nullptr, tr("Aborted"), tr("The mod updater was aborted!"), QMessageBox::Warning)->show();
        return;
    }
    if (updateDialog.noUpdates()) {
        CustomMessageBox::selectable(nullptr, tr("Update checker"), tr("All mods are up-to-date! :)"))->exec();
        return;
    }

    if (updateDialog.exec()) {
        auto tasks = updateDialog.getTasks();
        runDownloadTasks(nullptr, tasks, tr("Download Mods"));
        modsModel->update();
        scanMods();
    }
}

void InstanceViewModel::openModDownload()
{
    if (!m_instance)
        return;

    auto mcInstance = std::dynamic_pointer_cast<MinecraftInstance>(m_instance);
    if (!mcInstance)
        return;

    auto profile = mcInstance->getPackProfile();
    if (!profile || !profile->getModLoaders().has_value()) {
        QMessageBox::critical(nullptr, tr("Error"), tr("Please install a mod loader first!"));
        return;
    }

    auto modsModel = mcInstance->loaderModList();
    if (!modsModel)
        return;

    ResourceDownload::ModDownloadDialog dialog(nullptr, modsModel, m_instance.get());
    if (dialog.exec()) {
        runDownloadTasks(nullptr, dialog.getTasks(), tr("Download Mods"));
        modsModel->update();
        scanMods();
    }
}

void InstanceViewModel::resetModMetadata(const QVariantList& indices)
{
    if (!m_instance)
        return;

    auto mcInstance = std::dynamic_pointer_cast<MinecraftInstance>(m_instance);
    if (!mcInstance)
        return;

    auto modsModel = mcInstance->loaderModList();
    if (!modsModel)
        return;

    QModelIndexList indexList;
    for (const auto& entry : indices) {
        const int idx = entry.toInt();
        if (idx >= 0 && idx < modsModel->rowCount()) {
            indexList << modsModel->index(idx, 0);
        }
    }

    if (!indexList.isEmpty()) {
        modsModel->deleteMetadata(indexList);
        modsModel->update();
        scanMods();
    }
}

void InstanceViewModel::verifyDependencies()
{
    if (!m_instance)
        return;

    auto mcInstance = std::dynamic_pointer_cast<MinecraftInstance>(m_instance);
    if (!mcInstance)
        return;

    auto profile = mcInstance->getPackProfile();
    if (!profile || !profile->getModLoaders().has_value()) {
        QMessageBox::critical(nullptr, tr("Error"), tr("Please install a mod loader first!"));
        return;
    }
    if (APPLICATION->settings()->get("ModMetadataDisabled").toBool()) {
        QMessageBox::critical(nullptr, tr("Error"), tr("Mod updates are unavailable when metadata is disabled!"));
        return;
    }
    if (m_instance->isRunning()) {
        auto response = CustomMessageBox::selectable(nullptr, tr("Confirm Update"),
                                                     tr("Updating mods while the game is running may cause mod duplication and game crashes.\n"
                                                        "The old files may not be deleted as they are in use.\n"
                                                        "Are you sure you want to do this?"),
                                                     QMessageBox::Warning, QMessageBox::Yes | QMessageBox::No, QMessageBox::No)
                            ->exec();
        if (response != QMessageBox::Yes)
            return;
    }

    auto modsModel = mcInstance->loaderModList();
    if (!modsModel)
        return;

    auto modsList = modsModel->allResources();
    if (modsList.isEmpty())
        return;

    ResourceUpdateDialog updateDialog(nullptr, m_instance.get(), modsModel, modsList, true, profile->getModLoadersList());
    updateDialog.checkCandidates();

    if (updateDialog.aborted()) {
        CustomMessageBox::selectable(nullptr, tr("Aborted"), tr("The mod updater was aborted!"), QMessageBox::Warning)->show();
        return;
    }
    if (updateDialog.noUpdates()) {
        CustomMessageBox::selectable(nullptr, tr("Update checker"), tr("All mods are up-to-date! :)"))->exec();
        return;
    }

    if (updateDialog.exec()) {
        auto tasks = updateDialog.getTasks();
        runDownloadTasks(nullptr, tasks, tr("Download Mods"));
        modsModel->update();
        scanMods();
    }
}

void InstanceViewModel::exportModList()
{
    if (!m_instance)
        return;

    auto mcInstance = std::dynamic_pointer_cast<MinecraftInstance>(m_instance);
    if (!mcInstance)
        return;

    auto modsModel = mcInstance->loaderModList();
    if (!modsModel)
        return;

    ExportToModListDialog dialog(m_instance->name(), modsModel->allMods());
    dialog.exec();
}

void InstanceViewModel::scanMods()
{
    m_modsModel.clear();

    if (!m_instance) {
        emit modsModelChanged();
        return;
    }

    auto mcInstance = std::dynamic_pointer_cast<MinecraftInstance>(m_instance);
    if (!mcInstance) {
        emit modsModelChanged();
        return;
    }

    auto modsModel = mcInstance->loaderModList();
    if (!modsModel) {
        emit modsModelChanged();
        return;
    }

    QRegularExpression filter;
    bool hasFilter = !m_modFilter.trimmed().isEmpty();
    if (hasFilter) {
        filter = QRegularExpression(m_modFilter, QRegularExpression::CaseInsensitiveOption);
        if (!filter.isValid()) {
            hasFilter = false;
        }
    }

    for (int i = 0; i < modsModel->rowCount(); i++) {
        auto& mod = modsModel->at(i);
        if (hasFilter && !mod.applyFilter(filter))
            continue;

        QVariantMap item;
        item["name"] = mod.name();
        item["version"] = mod.version();
        item["description"] = mod.description();
        item["enabled"] = mod.enabled();
        item["fileName"] = mod.fileinfo().fileName();
        item["filename"] = mod.fileinfo().fileName();
        item["authors"] = mod.authors().join(", ");
        item["iconPath"] = mod.iconPath();

        m_modsModel.append(item);
    }

    emit modsModelChanged();
}

// ============ Resource Packs ============

QVariantList InstanceViewModel::resourcePacksModel() const
{
    return m_resourcePacksModel;
}

int InstanceViewModel::resourcePacksCount() const
{
    return m_resourcePacksModel.size();
}

void InstanceViewModel::refreshResourcePacks()
{
    scanResourcePacks();
}

void InstanceViewModel::addResourcePack(const QString& filePath)
{
    if (!m_instance || filePath.isEmpty())
        return;

    auto mcInstance = std::dynamic_pointer_cast<MinecraftInstance>(m_instance);
    if (!mcInstance)
        return;

    auto model = mcInstance->resourcePackList();
    if (model) {
        model->installResource(filePath);
        scanResourcePacks();
    }
}

void InstanceViewModel::removeResourcePack(int index)
{
    if (!m_instance || index < 0 || index >= m_resourcePacksModel.size())
        return;

    auto mcInstance = std::dynamic_pointer_cast<MinecraftInstance>(m_instance);
    if (!mcInstance)
        return;

    auto model = mcInstance->resourcePackList();
    if (model) {
        model->deleteResources(QModelIndexList() << model->index(index, 0));
        scanResourcePacks();
    }
}

void InstanceViewModel::enableResourcePack(int index, bool enabled)
{
    if (!m_instance || index < 0 || index >= m_resourcePacksModel.size())
        return;

    auto mcInstance = std::dynamic_pointer_cast<MinecraftInstance>(m_instance);
    if (!mcInstance)
        return;

    auto model = mcInstance->resourcePackList();
    if (model) {
        model->setResourceEnabled(QModelIndexList() << model->index(index, 0), enabled ? EnableAction::ENABLE : EnableAction::DISABLE);
        scanResourcePacks();
    }
}

void InstanceViewModel::browseForResourcePacks()
{
    if (!m_instance)
        return;

    QString startDir = gameRoot() + "/resourcepacks";
    QStringList files = QFileDialog::getOpenFileNames(nullptr, tr("Select Resource Packs"), startDir,
                                                      tr("Resource Packs (*.zip *.mcpack);;All Files (*)"));
    for (const auto& path : files) {
        addResourcePack(path);
    }
}

void InstanceViewModel::openResourcePackDownload()
{
    if (!m_instance)
        return;

    auto mcInstance = std::dynamic_pointer_cast<MinecraftInstance>(m_instance);
    if (!mcInstance)
        return;

    auto model = mcInstance->resourcePackList();
    if (!model)
        return;

    ResourceDownload::ResourcePackDownloadDialog dialog(nullptr, model, m_instance.get());
    if (dialog.exec()) {
        runDownloadTasks(nullptr, dialog.getTasks(), tr("Download Resource Packs"));
        model->update();
        scanResourcePacks();
    }
}

void InstanceViewModel::scanResourcePacks()
{
    m_resourcePacksModel.clear();

    if (!m_instance) {
        emit resourcePacksModelChanged();
        return;
    }

    auto mcInstance = std::dynamic_pointer_cast<MinecraftInstance>(m_instance);
    if (!mcInstance) {
        emit resourcePacksModelChanged();
        return;
    }

    auto model = mcInstance->resourcePackList();
    if (!model) {
        emit resourcePacksModelChanged();
        return;
    }

    for (int i = 0; i < model->rowCount(); i++) {
        auto& resource = model->at(i);
        QVariantMap item;
        item["name"] = resource.name();
        item["description"] = resource.description();
        item["enabled"] = resource.enabled();
        item["fileName"] = resource.fileinfo().fileName();
        item["filename"] = resource.fileinfo().fileName();

        m_resourcePacksModel.append(item);
    }

    emit resourcePacksModelChanged();
}

// ============ Shader Packs ============

QVariantList InstanceViewModel::shaderPacksModel() const
{
    return m_shaderPacksModel;
}

int InstanceViewModel::shaderPacksCount() const
{
    return m_shaderPacksModel.size();
}

void InstanceViewModel::refreshShaderPacks()
{
    scanShaderPacks();
}

void InstanceViewModel::addShaderPack(const QString& filePath)
{
    if (!m_instance || filePath.isEmpty())
        return;

    auto mcInstance = std::dynamic_pointer_cast<MinecraftInstance>(m_instance);
    if (!mcInstance)
        return;

    auto model = mcInstance->shaderPackList();
    if (model) {
        model->installResource(filePath);
        scanShaderPacks();
    }
}

void InstanceViewModel::removeShaderPack(int index)
{
    if (!m_instance || index < 0 || index >= m_shaderPacksModel.size())
        return;

    auto mcInstance = std::dynamic_pointer_cast<MinecraftInstance>(m_instance);
    if (!mcInstance)
        return;

    auto model = mcInstance->shaderPackList();
    if (model) {
        model->deleteResources(QModelIndexList() << model->index(index, 0));
        scanShaderPacks();
    }
}

void InstanceViewModel::enableShaderPack(int index, bool enabled)
{
    if (!m_instance || index < 0 || index >= m_shaderPacksModel.size())
        return;

    auto mcInstance = std::dynamic_pointer_cast<MinecraftInstance>(m_instance);
    if (!mcInstance)
        return;

    auto model = mcInstance->shaderPackList();
    if (model) {
        model->setResourceEnabled(QModelIndexList() << model->index(index, 0), enabled ? EnableAction::ENABLE : EnableAction::DISABLE);
        scanShaderPacks();
    }
}

void InstanceViewModel::browseForShaderPacks()
{
    if (!m_instance)
        return;

    QString startDir = gameRoot() + "/shaderpacks";
    QStringList files =
        QFileDialog::getOpenFileNames(nullptr, tr("Select Shader Packs"), startDir, tr("Shader Packs (*.zip);;All Files (*)"));
    for (const auto& path : files) {
        addShaderPack(path);
    }
}

void InstanceViewModel::openShaderPackDownload()
{
    if (!m_instance)
        return;

    auto mcInstance = std::dynamic_pointer_cast<MinecraftInstance>(m_instance);
    if (!mcInstance)
        return;

    auto model = mcInstance->shaderPackList();
    if (!model)
        return;

    ResourceDownload::ShaderPackDownloadDialog dialog(nullptr, model, m_instance.get());
    if (dialog.exec()) {
        runDownloadTasks(nullptr, dialog.getTasks(), tr("Download Shader Packs"));
        model->update();
        scanShaderPacks();
    }
}

void InstanceViewModel::scanShaderPacks()
{
    m_shaderPacksModel.clear();

    if (!m_instance) {
        emit shaderPacksModelChanged();
        return;
    }

    auto mcInstance = std::dynamic_pointer_cast<MinecraftInstance>(m_instance);
    if (!mcInstance) {
        emit shaderPacksModelChanged();
        return;
    }

    auto model = mcInstance->shaderPackList();
    if (!model) {
        emit shaderPacksModelChanged();
        return;
    }

    for (int i = 0; i < model->rowCount(); i++) {
        auto& resource = model->at(i);
        QVariantMap item;
        item["name"] = resource.name();
        item["enabled"] = resource.enabled();
        item["fileName"] = resource.fileinfo().fileName();
        item["filename"] = resource.fileinfo().fileName();

        m_shaderPacksModel.append(item);
    }

    emit shaderPacksModelChanged();
}

// ============ Texture Packs ============

QVariantList InstanceViewModel::texturePacksModel() const
{
    return m_texturePacksModel;
}

int InstanceViewModel::texturePacksCount() const
{
    return m_texturePacksModel.size();
}

void InstanceViewModel::refreshTexturePacks()
{
    scanTexturePacks();
}

void InstanceViewModel::addTexturePack(const QString& filePath)
{
    if (!m_instance || filePath.isEmpty())
        return;

    auto mcInstance = std::dynamic_pointer_cast<MinecraftInstance>(m_instance);
    if (!mcInstance)
        return;

    auto model = mcInstance->texturePackList();
    if (model) {
        model->installResource(filePath);
        scanTexturePacks();
    }
}

void InstanceViewModel::removeTexturePack(int index)
{
    if (!m_instance || index < 0 || index >= m_texturePacksModel.size())
        return;

    auto mcInstance = std::dynamic_pointer_cast<MinecraftInstance>(m_instance);
    if (!mcInstance)
        return;

    auto model = mcInstance->texturePackList();
    if (model) {
        model->deleteResources(QModelIndexList() << model->index(index, 0));
        scanTexturePacks();
    }
}

void InstanceViewModel::enableTexturePack(int index, bool enabled)
{
    if (!m_instance || index < 0 || index >= m_texturePacksModel.size())
        return;

    auto mcInstance = std::dynamic_pointer_cast<MinecraftInstance>(m_instance);
    if (!mcInstance)
        return;

    auto model = mcInstance->texturePackList();
    if (model) {
        model->setResourceEnabled(QModelIndexList() << model->index(index, 0), enabled ? EnableAction::ENABLE : EnableAction::DISABLE);
        scanTexturePacks();
    }
}

void InstanceViewModel::browseForTexturePacks()
{
    if (!m_instance)
        return;

    QString startDir = gameRoot() + "/texturepacks";
    QStringList files = QFileDialog::getOpenFileNames(nullptr, tr("Select Texture Packs"), startDir,
                                                      tr("Texture Packs (*.zip);;All Files (*)"));
    for (const auto& path : files) {
        addTexturePack(path);
    }
}

void InstanceViewModel::scanTexturePacks()
{
    m_texturePacksModel.clear();

    if (!m_instance) {
        emit texturePacksModelChanged();
        return;
    }

    auto mcInstance = std::dynamic_pointer_cast<MinecraftInstance>(m_instance);
    if (!mcInstance) {
        emit texturePacksModelChanged();
        return;
    }

    auto model = mcInstance->texturePackList();
    if (!model) {
        emit texturePacksModelChanged();
        return;
    }

    for (int i = 0; i < model->rowCount(); i++) {
        auto& resource = model->at(i);
        QVariantMap item;
        item["name"] = resource.name();
        item["enabled"] = resource.enabled();
        item["fileName"] = resource.fileinfo().fileName();
        item["filename"] = resource.fileinfo().fileName();

        m_texturePacksModel.append(item);
    }

    emit texturePacksModelChanged();
}

// ============ Data Packs ============

QVariantList InstanceViewModel::dataPacksModel() const
{
    return m_dataPacksModel;
}

int InstanceViewModel::dataPacksCount() const
{
    return m_dataPacksModel.size();
}

void InstanceViewModel::refreshDataPacks()
{
    if (m_dataPacksFolderModel) {
        m_dataPacksFolderModel->update();
    }
    scanDataPacks();
}

void InstanceViewModel::browseForDataPacks()
{
    if (!m_instance || !m_dataPacksFolderModel) {
        return;
    }

    QString startDir = m_selectedDataPacksPath;
    QStringList files =
        QFileDialog::getOpenFileNames(nullptr, tr("Select Data Packs"), startDir, tr("Data Packs (*.zip);;All Files (*)"));

    if (files.isEmpty()) {
        QString dir = QFileDialog::getExistingDirectory(nullptr, tr("Select Data Pack Folder"), startDir);
        if (!dir.isEmpty()) {
            files << dir;
        }
    }

    for (const auto& path : files) {
        m_dataPacksFolderModel->installResource(path);
    }

    if (!files.isEmpty()) {
        m_dataPacksFolderModel->update();
        scanDataPacks();
    }
}

void InstanceViewModel::openDataPackDownload()
{
    if (!m_instance || !m_dataPacksFolderModel)
        return;

    ResourceDownload::DataPackDownloadDialog dialog(nullptr, m_dataPacksFolderModel, m_instance.get());
    if (dialog.exec()) {
        runDownloadTasks(nullptr, dialog.getTasks(), tr("Download Data Packs"));
        m_dataPacksFolderModel->update();
        scanDataPacks();
    }
}

void InstanceViewModel::selectWorldForDataPacks(int index)
{
    m_selectedDataPacksPath.clear();
    m_dataPacksFolderModel.reset();
    m_dataPacksModel.clear();

    if (!m_instance || index < 0 || index >= m_worldPaths.size()) {
        emit dataPacksModelChanged();
        return;
    }

    QString path = FS::PathCombine(m_worldPaths.at(index), "datapacks");
    QDir dir(path);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    bool isIndexed = !APPLICATION->settings()->get("ModMetadataDisabled").toBool();
    m_dataPacksFolderModel = std::make_shared<DataPackFolderModel>(path, m_instance.get(), isIndexed, true);
    connect(m_dataPacksFolderModel.get(), &ResourceFolderModel::updateFinished, this, &InstanceViewModel::scanDataPacks);
    m_selectedDataPacksPath = path;

    m_dataPacksFolderModel->update();
    scanDataPacks();
}

void InstanceViewModel::setDataPackEnabled(int index, bool enabled)
{
    if (!m_dataPacksFolderModel || index < 0 || index >= m_dataPacksFolderModel->rowCount())
        return;

    m_dataPacksFolderModel->setResourceEnabled(QModelIndexList() << m_dataPacksFolderModel->index(index, 0),
                                               enabled ? EnableAction::ENABLE : EnableAction::DISABLE);
    scanDataPacks();
}

void InstanceViewModel::deleteDataPack(int index)
{
    if (!m_dataPacksFolderModel || index < 0 || index >= m_dataPacksFolderModel->rowCount())
        return;

    m_dataPacksFolderModel->deleteResources(QModelIndexList() << m_dataPacksFolderModel->index(index, 0));
    scanDataPacks();
}

void InstanceViewModel::scanDataPacks()
{
    m_dataPacksModel.clear();

    if (!m_dataPacksFolderModel) {
        emit dataPacksModelChanged();
        return;
    }

    for (int i = 0; i < m_dataPacksFolderModel->rowCount(); i++) {
        auto& pack = m_dataPacksFolderModel->at(i);
        QVariantMap item;
        item["name"] = pack.name();
        item["description"] = pack.description();
        item["enabled"] = pack.enabled();
        item["fileName"] = pack.fileinfo().fileName();
        item["filename"] = pack.fileinfo().fileName();
        item["iconPath"] = QString();

        m_dataPacksModel.append(item);
    }

    emit dataPacksModelChanged();
}

// ============ Other Logs ============

QStringList InstanceViewModel::otherLogsList() const
{
    return m_otherLogsList;
}

QString InstanceViewModel::otherLogContent() const
{
    return m_otherLogContent;
}

void InstanceViewModel::loadOtherLog(int index)
{
    QString content;
    m_currentOtherLogPath.clear();

    if (index < 0 || index >= m_otherLogPaths.size()) {
        if (m_otherLogContent != content) {
            m_otherLogContent = content;
            emit otherLogContentChanged();
        }
        return;
    }

    QString path = m_otherLogPaths.at(index);
    QFile file(path);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        constexpr qint64 kMaxBytes = 1024 * 1024;
        if (file.size() > kMaxBytes) {
            file.seek(file.size() - kMaxBytes);
        }
        content = QString::fromUtf8(file.readAll());
        m_currentOtherLogPath = path;
    }

    if (m_otherLogContent != content) {
        m_otherLogContent = content;
        emit otherLogContentChanged();
    }
}

void InstanceViewModel::deleteSelectedLog(int index)
{
    if (index < 0 || index >= m_otherLogPaths.size()) {
        return;
    }

    QString path = m_otherLogPaths.at(index);
    if (QFile::remove(path)) {
        if (m_currentOtherLogPath == path) {
            m_currentOtherLogPath.clear();
            m_otherLogContent.clear();
            emit otherLogContentChanged();
        }
        scanOtherLogs();
    }
}

void InstanceViewModel::deleteAllLogs()
{
    for (const auto& path : m_otherLogPaths) {
        QFile::remove(path);
    }
    m_currentOtherLogPath.clear();
    m_otherLogContent.clear();
    emit otherLogContentChanged();
    scanOtherLogs();
}

void InstanceViewModel::copyOtherLogToClipboard()
{
    if (m_otherLogContent.isEmpty())
        return;

    if (auto clipboard = QGuiApplication::clipboard()) {
        clipboard->setText(m_otherLogContent);
    }
}

void InstanceViewModel::uploadOtherLog()
{
    copyOtherLogToClipboard();
}

void InstanceViewModel::reloadOtherLog()
{
    if (m_currentOtherLogPath.isEmpty()) {
        loadOtherLog(0);
        return;
    }

    int index = m_otherLogPaths.indexOf(m_currentOtherLogPath);
    if (index >= 0) {
        loadOtherLog(index);
    }
}

void InstanceViewModel::findInOtherLog(const QString& text)
{
    if (text.isEmpty()) {
        return;
    }

    const int index = m_otherLogContent.indexOf(text, 0, Qt::CaseInsensitive);
    if (index >= 0) {
        qDebug() << "[InstanceViewModel] Found text in other log at index" << index << "for instance" << m_instanceId;
    } else {
        qDebug() << "[InstanceViewModel] Text not found in other log for instance" << m_instanceId;
    }
}

void InstanceViewModel::scanOtherLogs()
{
    m_otherLogsList.clear();
    m_otherLogPaths.clear();

    if (!m_instance) {
        emit otherLogsListChanged();
        return;
    }

    struct LogEntry {
        QString display;
        QString path;
        QDateTime modified;
    };

    QList<LogEntry> entries;
    QString root = gameRoot();
    QStringList directories = { FS::PathCombine(root, "logs"), FS::PathCombine(root, "crash-reports") };
    QStringList filters = { "*.log", "*.txt", "*.out" };

    for (const auto& dirPath : directories) {
        QDir dir(dirPath);
        if (!dir.exists()) {
            continue;
        }

        QFileInfoList files = dir.entryInfoList(filters, QDir::Files, QDir::Time);
        for (const auto& file : files) {
            LogEntry entry;
            entry.path = file.absoluteFilePath();
            entry.display = QDir(root).relativeFilePath(entry.path);
            entry.modified = file.lastModified();
            entries.append(entry);
        }
    }

    std::sort(entries.begin(), entries.end(), [](const LogEntry& a, const LogEntry& b) {
        return a.modified > b.modified;
    });

    for (const auto& entry : entries) {
        m_otherLogsList.append(entry.display);
        m_otherLogPaths.append(entry.path);
    }

    if (!m_currentOtherLogPath.isEmpty() && !m_otherLogPaths.contains(m_currentOtherLogPath)) {
        m_currentOtherLogPath.clear();
        m_otherLogContent.clear();
        emit otherLogContentChanged();
    }

    emit otherLogsListChanged();
}
