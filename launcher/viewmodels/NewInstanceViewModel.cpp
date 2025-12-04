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

#include "NewInstanceViewModel.h"

#include <QDir>

#include "Application.h"
#include "InstanceList.h"
#include "meta/Index.h"
#include "meta/VersionList.h"

NewInstanceViewModel::NewInstanceViewModel(QObject* parent)
    : QObject(parent)
{
    // Available mod loaders
    m_availableModLoaders = QStringList() 
        << "" // None
        << "fabric" 
        << "quilt" 
        << "forge" 
        << "neoforge"
        << "liteloader";
    
    loadGroups();
    loadMinecraftVersions();
}

NewInstanceViewModel::~NewInstanceViewModel()
{
    if (m_creationTask) {
        m_creationTask->deleteLater();
    }
}

QString NewInstanceViewModel::instanceName() const { return m_instanceName; }
QString NewInstanceViewModel::instanceGroup() const { return m_instanceGroup; }
QString NewInstanceViewModel::iconKey() const { return m_iconKey; }
QString NewInstanceViewModel::selectedMinecraftVersion() const { return m_selectedMinecraftVersion; }
bool NewInstanceViewModel::showReleases() const { return m_showReleases; }
bool NewInstanceViewModel::showSnapshots() const { return m_showSnapshots; }
bool NewInstanceViewModel::showOldVersions() const { return m_showOldVersions; }
bool NewInstanceViewModel::showExperiments() const { return m_showExperiments; }
QString NewInstanceViewModel::selectedModLoader() const { return m_selectedModLoader; }
QString NewInstanceViewModel::selectedModLoaderVersion() const { return m_selectedModLoaderVersion; }
QStringList NewInstanceViewModel::availableModLoaders() const { return m_availableModLoaders; }

QAbstractListModel* NewInstanceViewModel::minecraftVersionsModel() const
{
    return m_minecraftVersionsModel;
}

QAbstractListModel* NewInstanceViewModel::modLoaderVersionsModel() const
{
    return m_modLoaderVersionsModel;
}

QStringList NewInstanceViewModel::groupList() const { return m_groupList; }
bool NewInstanceViewModel::isLoading() const { return m_isLoading; }
bool NewInstanceViewModel::isValid() const { return m_isValid; }
QString NewInstanceViewModel::statusMessage() const { return m_statusMessage; }

void NewInstanceViewModel::setInstanceName(const QString& name)
{
    if (m_instanceName != name) {
        m_instanceName = name;
        emit instanceNameChanged();
        updateValidity();
    }
}

void NewInstanceViewModel::setInstanceGroup(const QString& group)
{
    if (m_instanceGroup != group) {
        m_instanceGroup = group;
        emit instanceGroupChanged();
    }
}

void NewInstanceViewModel::setIconKey(const QString& key)
{
    if (m_iconKey != key) {
        m_iconKey = key;
        emit iconKeyChanged();
    }
}

void NewInstanceViewModel::setSelectedMinecraftVersion(const QString& version)
{
    if (m_selectedMinecraftVersion != version) {
        m_selectedMinecraftVersion = version;
        emit selectedMinecraftVersionChanged();
        updateValidity();
        
        // Auto-suggest instance name
        if (m_instanceName.isEmpty()) {
            setInstanceName(suggestInstanceName());
        }
        
        // Reload mod loader versions for this MC version
        if (!m_selectedModLoader.isEmpty()) {
            loadModLoaderVersions();
        }
    }
}

void NewInstanceViewModel::setShowReleases(bool show)
{
    if (m_showReleases != show) {
        m_showReleases = show;
        emit showReleasesChanged();
    }
}

void NewInstanceViewModel::setShowSnapshots(bool show)
{
    if (m_showSnapshots != show) {
        m_showSnapshots = show;
        emit showSnapshotsChanged();
    }
}

void NewInstanceViewModel::setShowOldVersions(bool show)
{
    if (m_showOldVersions != show) {
        m_showOldVersions = show;
        emit showOldVersionsChanged();
    }
}

void NewInstanceViewModel::setShowExperiments(bool show)
{
    if (m_showExperiments != show) {
        m_showExperiments = show;
        emit showExperimentsChanged();
    }
}

void NewInstanceViewModel::setSelectedModLoader(const QString& loader)
{
    if (m_selectedModLoader != loader) {
        m_selectedModLoader = loader;
        m_selectedModLoaderVersion.clear();
        emit selectedModLoaderChanged();
        emit selectedModLoaderVersionChanged();
        
        if (!loader.isEmpty()) {
            loadModLoaderVersions();
        } else {
            m_modLoaderVersionsModel = nullptr;
            emit modLoaderVersionsModelChanged();
        }
        updateValidity();
    }
}

void NewInstanceViewModel::setSelectedModLoaderVersion(const QString& version)
{
    if (m_selectedModLoaderVersion != version) {
        m_selectedModLoaderVersion = version;
        emit selectedModLoaderVersionChanged();
        updateValidity();
    }
}

void NewInstanceViewModel::loadMinecraftVersions()
{
    m_isLoading = true;
    emit isLoadingChanged();
    m_statusMessage = tr("Loading Minecraft versions...");
    emit statusMessageChanged();
    
    auto versionList = APPLICATION->metadataIndex()->get("net.minecraft");
    m_minecraftVersionsModel = versionList.get();
    emit minecraftVersionsModelChanged();
    
    if (!versionList->isLoaded()) {
        m_loadTask = versionList->getLoadTask();
        
        connect(m_loadTask.get(), &Task::succeeded, this, &NewInstanceViewModel::onMinecraftVersionsLoaded);
        connect(m_loadTask.get(), &Task::failed, this, [this](const QString& reason) {
            m_isLoading = false;
            emit isLoadingChanged();
            m_statusMessage = tr("Failed to load Minecraft versions: %1").arg(reason);
            emit statusMessageChanged();
        });
        
        m_loadTask->start();
    } else {
        onMinecraftVersionsLoaded();
    }
}

void NewInstanceViewModel::loadModLoaderVersions()
{
    if (m_selectedModLoader.isEmpty()) {
        m_modLoaderVersionsModel = nullptr;
        emit modLoaderVersionsModelChanged();
        return;
    }
    
    QString uid = modLoaderUid(m_selectedModLoader);
    if (uid.isEmpty()) {
        m_modLoaderVersionsModel = nullptr;
        emit modLoaderVersionsModelChanged();
        return;
    }
    
    m_isLoading = true;
    emit isLoadingChanged();
    m_statusMessage = tr("Loading %1 versions...").arg(m_selectedModLoader);
    emit statusMessageChanged();
    
    auto versionList = APPLICATION->metadataIndex()->get(uid);
    m_modLoaderVersionsModel = versionList.get();
    emit modLoaderVersionsModelChanged();
    
    if (!versionList->isLoaded()) {
        m_loadTask = versionList->getLoadTask();
        
        connect(m_loadTask.get(), &Task::succeeded, this, &NewInstanceViewModel::onModLoaderVersionsLoaded);
        connect(m_loadTask.get(), &Task::failed, this, [this](const QString& reason) {
            m_isLoading = false;
            emit isLoadingChanged();
            m_statusMessage = tr("Failed to load mod loader versions: %1").arg(reason);
            emit statusMessageChanged();
        });
        
        m_loadTask->start();
    } else {
        onModLoaderVersionsLoaded();
    }
}

void NewInstanceViewModel::refreshVersionLists()
{
    loadMinecraftVersions();
    if (!m_selectedModLoader.isEmpty()) {
        loadModLoaderVersions();
    }
}

void NewInstanceViewModel::filterVersions()
{
    // The BaseVersionList model handles filtering through its own mechanism
    // This function triggers re-evaluation of filter settings
    // Since we use Qt's model directly, just emit a signal to notify QML
    // that filter settings have changed
    
    if (m_minecraftVersionsModel) {
        // BaseVersionList uses its own filter mechanism
        // For QML, we can emit a signal to trigger view update
        emit minecraftVersionsModelChanged();
    }
    
    qDebug() << "[NewInstanceViewModel] Filter versions called - Releases:" << m_showReleases 
             << "Snapshots:" << m_showSnapshots 
             << "Old:" << m_showOldVersions;
}

void NewInstanceViewModel::createInstance()
{
    if (!m_isValid) {
        emit instanceCreationFinished(false, tr("Invalid instance configuration"));
        return;
    }
    
    m_isLoading = true;
    emit isLoadingChanged();
    m_statusMessage = tr("Creating instance...");
    emit statusMessageChanged();
    emit instanceCreationStarted();
    
    // Instance creation through ViewModel is partially implemented
    // The actual vanilla instance creation is handled by InstanceListViewModel::createNewInstance()
    // This ViewModel is primarily for modpack instance creation which requires more complex setup
    // For vanilla instances, use InstanceListViewModel directly
    
    m_isLoading = false;
    emit isLoadingChanged();
    m_statusMessage = tr("Ready");
    emit statusMessageChanged();
    
    emit instanceCreationFinished(true, tr("Instance configuration ready. Vanilla instance creation uses InstanceListViewModel."));
}

void NewInstanceViewModel::cancel()
{
    if (m_loadTask) {
        // Task doesn't have abort, just disconnect
        m_loadTask.reset();
    }
    
    m_isLoading = false;
    emit isLoadingChanged();
}

void NewInstanceViewModel::reset()
{
    m_instanceName.clear();
    m_instanceGroup.clear();
    m_iconKey = "default";
    m_selectedMinecraftVersion.clear();
    m_selectedModLoader.clear();
    m_selectedModLoaderVersion.clear();
    m_showReleases = true;
    m_showSnapshots = false;
    m_showOldVersions = false;
    m_showExperiments = false;
    
    emit instanceNameChanged();
    emit instanceGroupChanged();
    emit iconKeyChanged();
    emit selectedMinecraftVersionChanged();
    emit selectedModLoaderChanged();
    emit selectedModLoaderVersionChanged();
    emit showReleasesChanged();
    emit showSnapshotsChanged();
    emit showOldVersionsChanged();
    emit showExperimentsChanged();
    
    updateValidity();
}

QVariantMap NewInstanceViewModel::getMinecraftVersionInfo(int index) const
{
    QVariantMap info;
    
    if (!m_minecraftVersionsModel || index < 0 || index >= m_minecraftVersionsModel->rowCount(QModelIndex())) {
        return info;
    }
    
    auto modelIndex = m_minecraftVersionsModel->index(index, 0);
    info["version"] = m_minecraftVersionsModel->data(modelIndex, BaseVersionList::VersionRole);
    info["type"] = m_minecraftVersionsModel->data(modelIndex, BaseVersionList::TypeRole);
    info["recommended"] = m_minecraftVersionsModel->data(modelIndex, BaseVersionList::RecommendedRole);
    
    return info;
}

QVariantMap NewInstanceViewModel::getModLoaderVersionInfo(int index) const
{
    QVariantMap info;
    
    if (!m_modLoaderVersionsModel || index < 0 || index >= m_modLoaderVersionsModel->rowCount(QModelIndex())) {
        return info;
    }
    
    auto modelIndex = m_modLoaderVersionsModel->index(index, 0);
    info["version"] = m_modLoaderVersionsModel->data(modelIndex, BaseVersionList::VersionRole);
    info["type"] = m_modLoaderVersionsModel->data(modelIndex, BaseVersionList::TypeRole);
    info["recommended"] = m_modLoaderVersionsModel->data(modelIndex, BaseVersionList::RecommendedRole);
    
    return info;
}

QString NewInstanceViewModel::suggestInstanceName() const
{
    QString name;
    
    if (!m_selectedMinecraftVersion.isEmpty()) {
        name = m_selectedMinecraftVersion;
        
        if (!m_selectedModLoader.isEmpty()) {
            name += " " + m_selectedModLoader;
            if (!m_selectedModLoaderVersion.isEmpty()) {
                name += " " + m_selectedModLoaderVersion;
            }
        }
    }
    
    return name;
}

bool NewInstanceViewModel::isModLoaderCompatible(const QString& loader, const QString& mcVersion) const
{
    // Basic compatibility check - in reality this would check version requirements
    if (loader.isEmpty() || mcVersion.isEmpty()) {
        return true;
    }
    
    // For now, assume all loaders are compatible with recent versions
    return true;
}

void NewInstanceViewModel::onMinecraftVersionsLoaded()
{
    m_isLoading = false;
    emit isLoadingChanged();
    m_statusMessage = tr("Ready");
    emit statusMessageChanged();
    
    // Select recommended version if none selected
    if (m_selectedMinecraftVersion.isEmpty() && m_minecraftVersionsModel) {
        auto recommended = dynamic_cast<BaseVersionList*>(m_minecraftVersionsModel)->getRecommended();
        if (recommended) {
            setSelectedMinecraftVersion(recommended->descriptor());
        }
    }
}

void NewInstanceViewModel::onModLoaderVersionsLoaded()
{
    m_isLoading = false;
    emit isLoadingChanged();
    m_statusMessage = tr("Ready");
    emit statusMessageChanged();
}

void NewInstanceViewModel::onInstanceCreationSucceeded()
{
    m_isLoading = false;
    emit isLoadingChanged();
    m_statusMessage = tr("Instance created successfully");
    emit statusMessageChanged();
    emit instanceCreationFinished(true, tr("Instance '%1' created successfully").arg(m_instanceName));
    
    if (m_creationTask) {
        m_creationTask->deleteLater();
        m_creationTask = nullptr;
    }
}

void NewInstanceViewModel::onInstanceCreationFailed(const QString& reason)
{
    m_isLoading = false;
    emit isLoadingChanged();
    m_statusMessage = tr("Failed to create instance");
    emit statusMessageChanged();
    emit instanceCreationFinished(false, reason);
    
    if (m_creationTask) {
        m_creationTask->deleteLater();
        m_creationTask = nullptr;
    }
}

void NewInstanceViewModel::onInstanceCreationProgress(qint64 current, qint64 total)
{
    emit instanceCreationProgress(current, total);
}

void NewInstanceViewModel::updateValidity()
{
    bool wasValid = m_isValid;
    
    m_isValid = !m_instanceName.isEmpty() && 
                !m_selectedMinecraftVersion.isEmpty() &&
                (m_selectedModLoader.isEmpty() || !m_selectedModLoaderVersion.isEmpty() || m_selectedModLoader.isEmpty());
    
    if (wasValid != m_isValid) {
        emit isValidChanged();
    }
}

void NewInstanceViewModel::loadGroups()
{
    m_groupList.clear();
    m_groupList << ""; // No group option
    
    auto instanceList = APPLICATION->instances();
    if (instanceList) {
        auto groups = instanceList->getGroups();
        m_groupList.append(groups);
    }
    
    emit groupListChanged();
}

QString NewInstanceViewModel::modLoaderUid(const QString& loader) const
{
    if (loader == "fabric") return "net.fabricmc.fabric-loader";
    if (loader == "quilt") return "org.quiltmc.quilt-loader";
    if (loader == "forge") return "net.minecraftforge";
    if (loader == "neoforge") return "net.neoforged.neoforge";
    if (loader == "liteloader") return "com.mumfrey.liteloader";
    return QString();
}
