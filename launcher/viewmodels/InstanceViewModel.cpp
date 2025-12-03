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

#include <QDesktopServices>
#include <QDir>
#include <QUrl>

#include "Application.h"
#include "InstanceList.h"

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
