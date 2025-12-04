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

#include "InstanceViewModel.h"

#include <QClipboard>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QGuiApplication>
#include <QImage>
#include <QMimeData>
#include <QUrl>

#include "Application.h"
#include "FileSystem.h"
#include "InstanceList.h"
#include "MMCZip.h"
#include "minecraft/MinecraftInstance.h"
#include "minecraft/PackProfile.h"
#include "minecraft/Component.h"
#include "minecraft/mod/ModFolderModel.h"
#include "minecraft/mod/ResourcePackFolderModel.h"
#include "minecraft/mod/ShaderPackFolderModel.h"
#include "minecraft/mod/TexturePackFolderModel.h"
#include "minecraft/mod/Resource.h"

// NBT library for servers.dat
#include <io/stream_reader.h>
#include <tag_compound.h>
#include <tag_list.h>
#include <tag_primitive.h>
#include <tag_string.h>
#include <sstream>

InstanceViewModel::InstanceViewModel(QObject* parent)
    : QObject(parent)
{
}

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
                connect(m_instance.get(), &BaseInstance::propertiesChanged, 
                        this, &InstanceViewModel::onInstancePropertiesChanged);
            }
        }
        
        emit instanceIdChanged();
        emit hasInstanceChanged();
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
    if (!m_instance) return QString();
    
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
    if (!m_instance) return false;
    auto settings = m_instance->settings();
    return settings ? settings->get("OverrideJavaLocation").toBool() : false;
}

void InstanceViewModel::setOverrideJava(bool override)
{
    if (!m_instance) return;
    auto settings = m_instance->settings();
    if (settings) {
        settings->set("OverrideJavaLocation", override);
        emit overrideJavaChanged();
    }
}

QString InstanceViewModel::javaPath() const
{
    if (!m_instance) return QString();
    auto settings = m_instance->settings();
    return settings ? settings->get("JavaPath").toString() : QString();
}

void InstanceViewModel::setJavaPath(const QString& path)
{
    if (!m_instance) return;
    auto settings = m_instance->settings();
    if (settings) {
        settings->set("JavaPath", path);
        emit javaPathChanged();
    }
}

QString InstanceViewModel::jvmArgs() const
{
    if (!m_instance) return QString();
    auto settings = m_instance->settings();
    return settings ? settings->get("JvmArgs").toString() : QString();
}

void InstanceViewModel::setJvmArgs(const QString& args)
{
    if (!m_instance) return;
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
              << "C:/Program Files/AdoptOpenJDK"
              << QDir::homePath() + "/.jdks";
#elif defined(Q_OS_MAC)
    javaPaths << "/Library/Java/JavaVirtualMachines"
              << "/System/Library/Frameworks/JavaVM.framework/Versions"
              << QDir::homePath() + "/Library/Java/JavaVirtualMachines";
#else
    javaPaths << "/usr/lib/jvm"
              << "/usr/lib64/jvm"
              << "/usr/local/lib/jvm"
              << QDir::homePath() + "/.jdks"
              << QDir::homePath() + "/.sdkman/candidates/java";
#endif

    QStringList foundJavas;
    for (const QString& basePath : javaPaths) {
        QDir dir(basePath);
        if (!dir.exists()) continue;
        
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
    if (!m_instance) return false;
    auto settings = m_instance->settings();
    return settings ? settings->get("OverrideMemory").toBool() : false;
}

void InstanceViewModel::setOverrideMemory(bool override)
{
    if (!m_instance) return;
    auto settings = m_instance->settings();
    if (settings) {
        settings->set("OverrideMemory", override);
        emit overrideMemoryChanged();
    }
}

int InstanceViewModel::minMemory() const
{
    if (!m_instance) return 512;
    auto settings = m_instance->settings();
    return settings ? settings->get("MinMemAlloc").toInt() : 512;
}

void InstanceViewModel::setMinMemory(int mb)
{
    if (!m_instance) return;
    auto settings = m_instance->settings();
    if (settings) {
        settings->set("MinMemAlloc", mb);
        emit minMemoryChanged();
    }
}

int InstanceViewModel::maxMemory() const
{
    if (!m_instance) return 4096;
    auto settings = m_instance->settings();
    return settings ? settings->get("MaxMemAlloc").toInt() : 4096;
}

void InstanceViewModel::setMaxMemory(int mb)
{
    if (!m_instance) return;
    auto settings = m_instance->settings();
    if (settings) {
        settings->set("MaxMemAlloc", mb);
        emit maxMemoryChanged();
    }
}

bool InstanceViewModel::overrideWindow() const
{
    if (!m_instance) return false;
    auto settings = m_instance->settings();
    return settings ? settings->get("OverrideWindow").toBool() : false;
}

void InstanceViewModel::setOverrideWindow(bool override)
{
    if (!m_instance) return;
    auto settings = m_instance->settings();
    if (settings) {
        settings->set("OverrideWindow", override);
        emit overrideWindowChanged();
    }
}

int InstanceViewModel::windowWidth() const
{
    if (!m_instance) return 854;
    auto settings = m_instance->settings();
    return settings ? settings->get("MinecraftWinWidth").toInt() : 854;
}

void InstanceViewModel::setWindowWidth(int width)
{
    if (!m_instance) return;
    auto settings = m_instance->settings();
    if (settings) {
        settings->set("MinecraftWinWidth", width);
        emit windowWidthChanged();
    }
}

int InstanceViewModel::windowHeight() const
{
    if (!m_instance) return 480;
    auto settings = m_instance->settings();
    return settings ? settings->get("MinecraftWinHeight").toInt() : 480;
}

void InstanceViewModel::setWindowHeight(int height)
{
    if (!m_instance) return;
    auto settings = m_instance->settings();
    if (settings) {
        settings->set("MinecraftWinHeight", height);
        emit windowHeightChanged();
    }
}

bool InstanceViewModel::maximizeWindow() const
{
    if (!m_instance) return false;
    auto settings = m_instance->settings();
    return settings ? settings->get("LaunchMaximized").toBool() : false;
}

void InstanceViewModel::setMaximizeWindow(bool maximize)
{
    if (!m_instance) return;
    auto settings = m_instance->settings();
    if (settings) {
        settings->set("LaunchMaximized", maximize);
        emit maximizeWindowChanged();
    }
}

bool InstanceViewModel::fullscreen() const
{
    if (!m_instance) return false;
    auto settings = m_instance->settings();
    return settings ? settings->get("LaunchFullscreen").toBool() : false;
}

void InstanceViewModel::setFullscreen(bool fs)
{
    if (!m_instance) return;
    auto settings = m_instance->settings();
    if (settings) {
        settings->set("LaunchFullscreen", fs);
        emit fullscreenChanged();
    }
}

bool InstanceViewModel::showConsole() const
{
    if (!m_instance) return true;
    auto settings = m_instance->settings();
    return settings ? settings->get("ShowConsole").toBool() : true;
}

void InstanceViewModel::setShowConsole(bool show)
{
    if (!m_instance) return;
    auto settings = m_instance->settings();
    if (settings) {
        settings->set("ShowConsole", show);
        emit showConsoleChanged();
    }
}

bool InstanceViewModel::closeOnLaunch() const
{
    if (!m_instance) return false;
    auto settings = m_instance->settings();
    return settings ? settings->get("CloseAfterLaunch").toBool() : false;
}

void InstanceViewModel::setCloseOnLaunch(bool close)
{
    if (!m_instance) return;
    auto settings = m_instance->settings();
    if (settings) {
        settings->set("CloseAfterLaunch", close);
        emit closeOnLaunchChanged();
    }
}

bool InstanceViewModel::quitAfterGame() const
{
    if (!m_instance) return false;
    auto settings = m_instance->settings();
    return settings ? settings->get("QuitAfterGameStop").toBool() : false;
}

void InstanceViewModel::setQuitAfterGame(bool quit)
{
    if (!m_instance) return;
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
    if (!m_instance) return;
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
    if (!m_instance) return;
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
    if (!m_instance) return;
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

void InstanceViewModel::loadFromInstance()
{
    emitAllChanged();
}

void InstanceViewModel::emitAllChanged()
{
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
        filters << "*.png" << "*.jpg" << "*.jpeg";
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
        scanMods();
    }
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
    
    for (int i = 0; i < modsModel->rowCount(); i++) {
        auto& mod = modsModel->at(i);
        QVariantMap item;
        item["name"] = mod.name();
        item["version"] = mod.version();
        item["description"] = mod.description();
        item["enabled"] = mod.enabled();
        item["filename"] = mod.fileinfo().fileName();
        item["authors"] = mod.authors().join(", ");
        
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
        model->setResourceEnabled(QModelIndexList() << model->index(index, 0), 
                                 enabled ? EnableAction::ENABLE : EnableAction::DISABLE);
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
        model->setResourceEnabled(QModelIndexList() << model->index(index, 0), 
                                 enabled ? EnableAction::ENABLE : EnableAction::DISABLE);
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
        model->setResourceEnabled(QModelIndexList() << model->index(index, 0), 
                                 enabled ? EnableAction::ENABLE : EnableAction::DISABLE);
        scanTexturePacks();
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
        item["filename"] = resource.fileinfo().fileName();
        
        m_texturePacksModel.append(item);
    }
    
    emit texturePacksModelChanged();
}
