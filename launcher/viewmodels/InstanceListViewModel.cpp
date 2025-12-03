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

#include "InstanceListViewModel.h"

#include "Application.h"
#include "BaseInstance.h"
#include "DesktopServices.h"
#include "InstanceList.h"
#include "QObjectPtr.h"
#include "FileSystem.h"
#include "MMCZip.h"
#include "icons/IconList.h"
#include "InstanceImportTask.h"
#include "minecraft/VanillaInstanceCreationTask.h"
#include "meta/Index.h"
#include <QFileInfo>
#include <QPixmap>
#include <QUrl>

namespace {
void saveInstanceIcon(const std::shared_ptr<BaseInstance>& instance)
{
    if (!instance) {
        return;
    }

    auto iconKey = instance->iconKey();
    auto iconList = APPLICATION->icons();
    auto mmcIcon = iconList ? iconList->icon(iconKey) : nullptr;
    if (!mmcIcon || mmcIcon->isBuiltIn()) {
        return;
    }

    auto path = mmcIcon->getFilePath();
    if (!path.isNull()) {
        QFileInfo inInfo(path);
        FS::copy(path, FS::PathCombine(instance->instanceRoot(), inInfo.fileName()))();
        return;
    }

    auto& image = mmcIcon->m_images[mmcIcon->type()];
    auto& icon = image.icon;
    auto sizes = icon.availableSizes();
    if (sizes.isEmpty()) {
        return;
    }

    auto areaOf = [](QSize size) { return size.width() * size.height(); };
    QSize largest = sizes[0];
    for (auto size : sizes) {
        if (areaOf(largest) < areaOf(size)) {
            largest = size;
        }
    }
    auto pixmap = icon.pixmap(largest);
    pixmap.save(FS::PathCombine(instance->instanceRoot(), iconKey + ".png"));
}
}  // namespace

InstanceListViewModel::InstanceListViewModel(QObject* parent) : QObject(parent) {}

int InstanceListViewModel::totalCount() const
{
    return m_totalCount;
}

QString InstanceListViewModel::selectedInstanceId() const
{
    return m_selectedInstanceId;
}

bool InstanceListViewModel::isBusy() const
{
    return m_busy;
}

QString InstanceListViewModel::busyReason() const
{
    return m_busyReason;
}

QStringList InstanceListViewModel::instanceNames() const
{
    return m_instanceNames;
}

QStringList InstanceListViewModel::instanceIds() const
{
    return m_instanceIds;
}

QStringList InstanceListViewModel::instanceIcons() const
{
    return m_instanceIcons;
}

QStringList InstanceListViewModel::instanceIconPaths() const
{
    return m_instanceIconPaths;
}

QStringList InstanceListViewModel::instanceGroups() const
{
    return m_instanceGroups;
}

QStringList InstanceListViewModel::instanceLastPlayed() const
{
    return m_instanceLastPlayed;
}

QStringList InstanceListViewModel::availableVersions() const
{
    return m_availableVersions;
}

bool InstanceListViewModel::hasSelection() const
{
    return !m_selectedInstanceId.isEmpty() && !m_currentInstance.expired();
}

bool InstanceListViewModel::canLaunchSelected() const
{
    return hasSelection() && m_canLaunchSelected;
}

bool InstanceListViewModel::isSelectedRunning() const
{
    return hasSelection() && m_isSelectedRunning;
}

bool InstanceListViewModel::canDeleteSelected() const
{
    return hasSelection() && m_canDeleteSelected;
}

bool InstanceListViewModel::canExportSelected() const
{
    return hasSelection() && m_canExportSelected;
}

bool InstanceListViewModel::canBackupSelected() const
{
    return hasSelection() && m_canBackupSelected;
}

void InstanceListViewModel::setTotalCount(int count)
{
    if (m_totalCount == count) {
        return;
    }
    m_totalCount = count;
    emit totalCountChanged();
}

void InstanceListViewModel::setSelectedInstanceId(const QString& id)
{
    if (m_selectedInstanceId == id) {
        return;
    }
    m_selectedInstanceId = id;
    bindInstanceSignals(resolveInstance(m_selectedInstanceId));
    emit selectedInstanceIdChanged();
    refreshInstanceState();
}

void InstanceListViewModel::setBusy(bool busy, const QString& reason)
{
    const bool busyStateChanged = m_busy != busy;
    bool reasonChanged = false;

    if (busy) {
        if (m_busyReason != reason) {
            m_busyReason = reason;
            reasonChanged = true;
        }
    } else if (!m_busyReason.isEmpty()) {
        m_busyReason.clear();
        reasonChanged = true;
    }

    if (busyStateChanged) {
        m_busy = busy;
    }

    if (busyStateChanged || reasonChanged) {
        emit busyChanged();
    }

    if (busyStateChanged) {
        if (m_busy) {
            emit started();
        } else {
            emit finished();
        }
    }
}

void InstanceListViewModel::setInstanceNames(const QStringList& names)
{
    if (m_instanceNames == names) {
        return;
    }
    m_instanceNames = names;
    emit instanceNamesChanged();
}

void InstanceListViewModel::setInstanceIds(const QStringList& ids)
{
    if (m_instanceIds == ids) {
        return;
    }
    m_instanceIds = ids;
    emit instanceListChanged();
}

void InstanceListViewModel::setInstanceIcons(const QStringList& icons)
{
    if (m_instanceIcons == icons) {
        return;
    }
    m_instanceIcons = icons;
    emit instanceListChanged();
}

void InstanceListViewModel::setInstanceGroups(const QStringList& groups)
{
    if (m_instanceGroups == groups) {
        return;
    }
    m_instanceGroups = groups;
    emit instanceListChanged();
}

void InstanceListViewModel::setInstanceLists(const QStringList& ids, const QStringList& names, const QStringList& icons,
                                             const QStringList& groups)
{
    m_instanceIds = ids;
    m_instanceNames = names;
    m_instanceIcons = icons;
    m_instanceIconPaths.clear();
    m_instanceIconPaths.reserve(icons.size());
    for (const auto& key : icons) {
        const MMCIcon* icon = APPLICATION->icons()->icon(key);
        QString path;
        if (icon) {
            path = icon->getFilePath();
        }
        m_instanceIconPaths.push_back(path);
    }
    m_instanceGroups = groups;
    emit instanceListChanged();
    emit instanceNamesChanged();
    refreshInstanceState();
}

void InstanceListViewModel::selectInstance(const QString& id)
{
    setSelectedInstanceId(id);
    emit instanceSelected(id);
}

void InstanceListViewModel::selectInstanceByIndex(int index)
{
    if (index < 0 || index >= m_instanceIds.size()) {
        return;
    }
    selectInstance(m_instanceIds.value(index));
}

void InstanceListViewModel::selectInstanceById(const QString& id)
{
    selectInstance(id);
}

void InstanceListViewModel::launchSelectedInstance()
{
    launchInstance(m_selectedInstanceId);
}

void InstanceListViewModel::launchInstance(const QString& id)
{
    if (id.isEmpty()) {
        emit errorOccurred(tr("No instance selected."));
        return;
    }
    auto instance = resolveInstance(id);
    if (!instance) {
        emit errorOccurred(tr("The selected instance could not be found."));
        return;
    }
    if (instance->isRunning()) {
        emit errorOccurred(tr("The selected instance is already running."));
        return;
    }
    APPLICATION->launch(instance);
}

void InstanceListViewModel::killSelectedInstance()
{
    if (m_selectedInstanceId.isEmpty()) {
        emit errorOccurred(tr("No instance selected."));
        return;
    }
    auto instance = resolveInstance(m_selectedInstanceId);
    if (!instance) {
        emit errorOccurred(tr("The selected instance could not be found."));
        return;
    }
    if (!instance->isRunning()) {
        emit errorOccurred(tr("The selected instance is not running."));
        return;
    }
    APPLICATION->kill(instance);
}

void InstanceListViewModel::openInstanceSettings(const QString& id)
{
    QString instanceId = id.isEmpty() ? m_selectedInstanceId : id;
    if (instanceId.isEmpty()) {
        emit errorOccurred(tr("No instance selected."));
        return;
    }
    auto instance = resolveInstance(instanceId);
    if (!instance) {
        emit errorOccurred(tr("The selected instance could not be found."));
        return;
    }
    APPLICATION->showInstanceWindow(instance);
}

void InstanceListViewModel::deleteSelectedInstance()
{
    deleteInstance(m_selectedInstanceId);
}

void InstanceListViewModel::deleteInstance(const QString& id)
{
    if (id.isEmpty()) {
        emit errorOccurred(tr("No instance selected."));
        return;
    }
    auto instances = instanceList();
    if (!instances) {
        emit errorOccurred(tr("Instance list is not available."));
        return;
    }

    setBusy(true, tr("Deleting instance"));

    const bool trashed = instances->trashInstance(id);
    if (!trashed) {
        instances->deleteInstance(id);
    }

    if (m_selectedInstanceId == id) {
        setSelectedInstanceId(QString());
    }

    setBusy(false);
    refreshInstanceState();
}

void InstanceListViewModel::refreshInstances()
{
    auto instances = instanceList();
    if (!instances) {
        emit errorOccurred(tr("Instance list is not available."));
        return;
    }
    setBusy(true, tr("Refreshing instances"));
    instances->loadList();
    
    // Populate the QStringLists from the instance list
    QStringList ids;
    QStringList names;
    QStringList icons;
    QStringList iconPaths;
    QStringList groups;
    QStringList lastPlayed;
    
    qDebug() << "[InstanceListViewModel::refreshInstances] Loading instances...";
    
    for (int i = 0; i < instances->count(); ++i) {
        auto instance = instances->at(i);
        if (!instance) continue;
        
        ids.append(instance->id());
        names.append(instance->name());
        icons.append(instance->iconKey());
        iconPaths.append(instance->iconKey());  // Will be resolved by icon system
        groups.append(instances->getInstanceGroup(instance->id()));
        
        // Format last played time
        qint64 lastLaunchMs = instance->lastLaunch();
        if (lastLaunchMs > 0) {
            QDateTime lastLaunch = QDateTime::fromMSecsSinceEpoch(lastLaunchMs);
            QDateTime now = QDateTime::currentDateTime();
            qint64 daysDiff = lastLaunch.daysTo(now);
            
            if (daysDiff == 0) {
                lastPlayed.append(tr("Today"));
            } else if (daysDiff == 1) {
                lastPlayed.append(tr("Yesterday"));
            } else if (daysDiff < 7) {
                lastPlayed.append(tr("%1 days ago").arg(daysDiff));
            } else if (daysDiff < 30) {
                lastPlayed.append(tr("%1 weeks ago").arg(daysDiff / 7));
            } else if (daysDiff < 365) {
                lastPlayed.append(tr("%1 months ago").arg(daysDiff / 30));
            } else {
                lastPlayed.append(lastLaunch.toString("MMM d, yyyy"));
            }
        } else {
            lastPlayed.append(tr("Never"));
        }
        
        qDebug() << "  Added instance:" << instance->id() << instance->name();
    }
    
    qDebug() << "[InstanceListViewModel::refreshInstances] Total instances:" << ids.count();
    
    // Load available Minecraft versions
    QStringList versions;
    versions << "Latest" << "1.20.1" << "1.20" << "1.19.2" << "1.19" << "1.18.2" << "1.18" << "1.17.1" << "1.16.5";
    if (m_availableVersions != versions) {
        m_availableVersions = versions;
        emit availableVersionsChanged();
    }
    
    m_instanceLastPlayed = lastPlayed;
    setInstanceLists(ids, names, icons, groups);
    setTotalCount(ids.count());
    
    setBusy(false);
    refreshInstanceState();
}

void InstanceListViewModel::renameSelectedInstance(const QString& newName)
{
    if (m_selectedInstanceId.isEmpty() || newName.isEmpty()) {
        return;
    }
    emit renameRequested(m_selectedInstanceId, newName);
}

void InstanceListViewModel::duplicateSelectedInstance(const QString& newIdOrName)
{
    if (m_selectedInstanceId.isEmpty() || newIdOrName.isEmpty()) {
        return;
    }
    emit duplicateRequested(m_selectedInstanceId, newIdOrName);
}

void InstanceListViewModel::renameInstance(const QString& id, const QString& newName)
{
    if (id.isEmpty() || newName.isEmpty()) {
        return;
    }
    auto instance = resolveInstance(id);
    if (!instance) {
        emit errorOccurred(tr("The selected instance could not be found."));
        return;
    }
    instance->setName(newName);
    APPLICATION->instances()->saveNow();
    refreshInstanceState();
}

void InstanceListViewModel::createNewInstance(const QString& name, const QString& version)
{
    if (name.isEmpty()) {
        emit errorOccurred(tr("Instance name is required."));
        return;
    }
    
    qDebug() << "[InstanceListViewModel::createNewInstance] Creating vanilla instance:" << name << "requested version:" << version;
    
    // Get the default group
    QString groupName = APPLICATION->settings()->get("LastUsedGroupForNewInstance").toString();
    
    // Get version from Meta index - use latest available Minecraft version
    auto versionIndex = APPLICATION->metadataIndex();
    if (!versionIndex) {
        emit errorOccurred(tr("Version list not available."));
        return;
    }
    
    auto mcVersions = versionIndex->get("net.minecraft");
    if (!mcVersions || mcVersions->versions().isEmpty()) {
        emit errorOccurred(tr("No Minecraft versions available."));
        return;
    }
    
    // Use the latest available version
    BaseVersion::Ptr selectedVersion = mcVersions->versions().first();
    
    if (!selectedVersion) {
        emit errorOccurred(tr("Failed to select a Minecraft version."));
        return;
    }
    
    qDebug() << "[InstanceListViewModel::createNewInstance] Using Minecraft version:" << selectedVersion->name();
    
    // Create vanilla instance task with the selected version
    auto task = new VanillaCreationTask(selectedVersion);
    if (task) {
        task->setName(name);
        task->setGroup(groupName);
        addInstance(task, tr("Creating instance: %1").arg(name));
        APPLICATION->settings()->set("LastUsedGroupForNewInstance", groupName);
    }
}

void InstanceListViewModel::importInstance(const QString& sourcePath, const QString& name)
{
    if (sourcePath.isEmpty()) {
        emit errorOccurred(tr("Source path is required."));
        return;
    }
    
    qDebug() << "[InstanceListViewModel::importInstance] Importing from:" << sourcePath << "name:" << name;
    
    // Get the default group
    QString groupName = APPLICATION->settings()->get("LastUsedGroupForNewInstance").toString();
    
    // Convert path to URL
    QUrl importUrl = QUrl::fromLocalFile(sourcePath);
    
    // Create import task
    auto task = new InstanceImportTask(importUrl, nullptr);  // nullptr = no parent widget
    if (!name.isEmpty()) {
        task->setName(name);
    }
    task->setGroup(groupName);
    
    addInstance(task, tr("Importing instance"));
    APPLICATION->settings()->set("LastUsedGroupForNewInstance", groupName);
}

void InstanceListViewModel::updateInstanceNotes(const QString& id, const QString& notes)
{
    if (id.isEmpty()) {
        return;
    }
    auto instance = resolveInstance(id);
    if (!instance) {
        emit errorOccurred(tr("The selected instance could not be found."));
        return;
    }
    instance->setNotes(notes);
    APPLICATION->instances()->saveNow();
}

void InstanceListViewModel::openInstanceFolder(const QString& id)
{
    QString instanceId = id.isEmpty() ? m_selectedInstanceId : id;
    if (instanceId.isEmpty()) {
        emit errorOccurred(tr("No instance selected."));
        return;
    }
    auto instance = resolveInstance(instanceId);
    if (!instance) {
        emit errorOccurred(tr("The selected instance could not be found."));
        return;
    }
    DesktopServices::openPath(instance->instanceRoot());
}

void InstanceListViewModel::updateInstanceIcon(const QString& id, const QString& iconKey)
{
    if (id.isEmpty() || iconKey.isEmpty()) {
        return;
    }
    auto instance = resolveInstance(id);
    if (!instance) {
        emit errorOccurred(tr("The selected instance could not be found."));
        return;
    }
    instance->setIconKey(iconKey);
    for (int i = 0; i < m_instanceIds.size(); ++i) {
        if (m_instanceIds[i] == id) {
            if (i < m_instanceIcons.size()) {
                m_instanceIcons[i] = iconKey;
            }
            if (i < m_instanceIconPaths.size()) {
                const MMCIcon* icon = APPLICATION->icons()->icon(iconKey);
                m_instanceIconPaths[i] = icon ? icon->getFilePath() : QString();
            }
            break;
        }
    }
    emit instanceListChanged();
}

void InstanceListViewModel::refreshInstanceMetadata(const QString& id)
{
    Q_UNUSED(id);
    refreshInstanceState();
}

void InstanceListViewModel::updateCustomProperty(const QString& id, const QString& key, const QVariant& value)
{
    if (id.isEmpty() || key.isEmpty()) {
        return;
    }
    auto instance = resolveInstance(id);
    if (!instance) {
        emit errorOccurred(tr("The selected instance could not be found."));
        return;
    }
    instance->setProperty(key.toUtf8().constData(), value);
    APPLICATION->instances()->saveNow();
}

void InstanceListViewModel::backupInstance(const QString& id, const QString& backupName, const BackupOptions& options)
{
    if (id.isEmpty()) {
        emit errorOccurred(tr("No instance selected."));
        return;
    }
    auto instance = resolveInstance(id);
    if (!instance) {
        emit errorOccurred(tr("The selected instance could not be found."));
        return;
    }
    if (backupName.isEmpty()) {
        emit errorOccurred(tr("Backup name is required."));
        return;
    }

    if (!m_backupManager) {
        m_backupManager = std::make_unique<BackupManager>(this);
        connect(m_backupManager.get(), &BackupManager::backupStarted, this, [this, id](const QString& instId, const QString&) {
            if (instId == id) {
                setBusy(true, tr("Creating backup"));
                emit started();
            }
        });
        connect(m_backupManager.get(), &BackupManager::backupCreated, this, [this, id](const QString& instId, const QString&) {
            if (instId == id) {
                setBusy(false);
                refreshInstanceState();
                emit finished();
            }
        });
        connect(m_backupManager.get(), &BackupManager::backupFailed, this, [this, id](const QString& instId, const QString& error) {
            if (instId == id) {
                emit errorOccurred(error);
                setBusy(false);
                refreshInstanceState();
            }
        });
    }

    m_backupManager->createBackupAsync(instance, backupName, options);
}

void InstanceListViewModel::createGroup(const QString& name)
{
    if (name.isEmpty()) {
        return;
    }

    auto list = instanceList();
    if (!list) {
        emit errorOccurred(tr("Unable to access instance list."));
        return;
    }

    // Creating a group implicitly happens when an instance is assigned to it.
    // If there is a current selection, assign it; otherwise just ensure the name is registered.
    if (!m_selectedInstanceId.isEmpty()) {
        list->setInstanceGroup(m_selectedInstanceId, name);
        for (int i = 0; i < m_instanceIds.size(); ++i) {
            if (m_instanceIds[i] == m_selectedInstanceId) {
                if (i < m_instanceGroups.size()) {
                    m_instanceGroups[i] = name;
                }
                break;
            }
        }
    }
    emit instanceListChanged();
}

void InstanceListViewModel::renameGroup(const QString& oldName, const QString& newName)
{
    if (oldName.isEmpty() || newName.isEmpty() || oldName == newName) {
        return;
    }

    auto list = instanceList();
    if (!list) {
        emit errorOccurred(tr("Unable to access instance list."));
        return;
    }

    list->renameGroup(oldName, newName);

    for (int i = 0; i < m_instanceGroups.size(); ++i) {
        if (m_instanceGroups[i] == oldName) {
            m_instanceGroups[i] = newName;
        }
    }
    emit instanceListChanged();
}

void InstanceListViewModel::deleteGroup(const QString& name)
{
    if (name.isEmpty()) {
        return;
    }

    auto list = instanceList();
    if (!list) {
        emit errorOccurred(tr("Unable to access instance list."));
        return;
    }

    list->deleteGroup(name);

    for (int i = 0; i < m_instanceGroups.size(); ++i) {
        if (m_instanceGroups[i] == name) {
            m_instanceGroups[i] = QString();
        }
    }
    emit instanceListChanged();
}

void InstanceListViewModel::moveInstanceToGroup(const QString& instanceId, const QString& groupName)
{
    if (instanceId.isEmpty()) {
        return;
    }
    auto list = instanceList();
    if (!list) {
        emit errorOccurred(tr("Unable to access instance list."));
        return;
    }

    list->setInstanceGroup(instanceId, groupName);
    for (int i = 0; i < m_instanceIds.size(); ++i) {
        if (m_instanceIds[i] == instanceId) {
            if (i < m_instanceGroups.size()) {
                m_instanceGroups[i] = groupName;
            }
            break;
        }
    }
    emit instanceListChanged();
}

bool InstanceListViewModel::handleDragDrop(const QString& instanceId, const QString& targetGroup)
{
    moveInstanceToGroup(instanceId, targetGroup);
    return true;
}

void InstanceListViewModel::reorderInstances(const QString& groupName, const QStringList& orderedInstanceIds)
{
    Q_UNUSED(groupName);
    Q_UNUSED(orderedInstanceIds);
    // Ordering persistence is handled by the underlying model; no-op placeholder for now.
}

void InstanceListViewModel::exportInstance(const QString& id, const QString& outputPath, const QFileInfoList& files)
{
    if (id.isEmpty()) {
        emit errorOccurred(tr("No instance selected."));
        return;
    }
    if (outputPath.isEmpty() || files.isEmpty()) {
        emit errorOccurred(tr("Nothing to export."));
        return;
    }

    auto instance = resolveInstance(id);
    if (!instance) {
        emit errorOccurred(tr("The selected instance could not be found."));
        return;
    }

    saveInstanceIcon(instance);
    auto task = new MMCZip::ExportToZipTask(outputPath, instance->instanceRoot(), files, QString(), true, true);
    startTask(task, tr("Exporting instance"));
}

void InstanceListViewModel::addInstance(InstanceTask* task, const QString& busyReason)
{
    startInstanceTask(task, busyReason.isEmpty() ? tr("Creating instance") : busyReason);
}

void InstanceListViewModel::copyInstance(InstanceTask* task, const QString& busyReason)
{
    startInstanceTask(task, busyReason.isEmpty() ? tr("Copying instance") : busyReason);
}

void InstanceListViewModel::createShortcut(const ShortcutUtils::Shortcut& shortcut)
{
    setBusy(true, tr("Creating shortcut"));
    switch (shortcut.target) {
        case ShortcutTarget::Desktop:
            ShortcutUtils::createInstanceShortcutOnDesktop(shortcut);
            break;
        case ShortcutTarget::Applications:
            ShortcutUtils::createInstanceShortcutInApplications(shortcut);
            break;
        case ShortcutTarget::Other:
        default:
            ShortcutUtils::createInstanceShortcutInOther(shortcut);
            break;
    }
    setBusy(false);
}

InstanceList* InstanceListViewModel::instanceList() const
{
    auto list = APPLICATION->instances();
    return list ? list.get() : nullptr;
}

std::shared_ptr<BaseInstance> InstanceListViewModel::resolveInstance(const QString& id) const
{
    auto list = APPLICATION->instances();
    if (!list) {
        return {};
    }
    return list->getInstanceById(id);
}

void InstanceListViewModel::bindInstanceSignals(const std::shared_ptr<BaseInstance>& instance)
{
    clearInstanceSignals();
    m_currentInstance = instance;
    if (instance) {
        m_runningConnection = connect(instance.get(), &BaseInstance::runningStatusChanged, this, [this] { refreshInstanceState(); });
    }
}

void InstanceListViewModel::clearInstanceSignals()
{
    if (m_runningConnection) {
        disconnect(m_runningConnection);
        m_runningConnection = {};
    }
    m_currentInstance.reset();
}

void InstanceListViewModel::refreshInstanceState()
{
    const auto instance = m_currentInstance.lock();
    bool nextCanLaunch = false;
    bool nextIsRunning = false;
    bool nextCanDelete = false;
    bool nextCanExport = false;
    bool nextCanBackup = false;

    if (instance) {
        nextIsRunning = instance->isRunning();
        nextCanLaunch = instance->canLaunch() && !nextIsRunning;
        nextCanDelete = !nextIsRunning;
        nextCanExport = instance->canExport();
        nextCanBackup = true;  // previously backed by selection-only enablement
    }

    const bool changed = nextCanLaunch != m_canLaunchSelected || nextIsRunning != m_isSelectedRunning ||
                         nextCanDelete != m_canDeleteSelected || nextCanExport != m_canExportSelected ||
                         nextCanBackup != m_canBackupSelected;

    m_canLaunchSelected = nextCanLaunch;
    m_isSelectedRunning = nextIsRunning;
    m_canDeleteSelected = nextCanDelete;
    m_canExportSelected = nextCanExport;
    m_canBackupSelected = nextCanBackup;

    if (changed) {
        emit instanceStateChanged();
    }
}

void InstanceListViewModel::startInstanceTask(InstanceTask* task, const QString& busyReason)
{
    if (!task) {
        emit errorOccurred(tr("Unable to start instance task."));
        return;
    }

    Task* ownedTask = APPLICATION->instances()->wrapInstanceTask(task);
    if (!ownedTask) {
        emit errorOccurred(tr("Failed to prepare instance task."));
        return;
    }

    startTask(ownedTask, busyReason);
}

void InstanceListViewModel::startTask(Task* task, const QString& busyReason)
{
    if (!task) {
        emit errorOccurred(tr("Unable to start task."));
        return;
    }

    setBusy(true, busyReason.isEmpty() ? tr("Working...") : busyReason);

    auto finish = [this, task](const QString& error) {
        if (!error.isEmpty()) {
            emit errorOccurred(error);
        }
        setBusy(false);
        refreshInstanceState();
        task->deleteLater();
    };

    connect(task, &Task::succeeded, this, [finish]() mutable { finish(QString()); });
    connect(task, &Task::failed, this, [finish](QString reason) mutable { finish(reason); });
    connect(task, &Task::aborted, this, [finish]() mutable { finish(tr("Task aborted.")); });
    task->start();
}

void InstanceListViewModel::setSelectedGroup(const QString& groupName)
{
    if (m_selectedInstanceId.isEmpty()) {
        emit errorOccurred(tr("No instance selected."));
        return;
    }
    moveInstanceToGroup(m_selectedInstanceId, groupName);
}

void InstanceListViewModel::exportSelectedInstance()
{
    if (m_selectedInstanceId.isEmpty()) {
        emit errorOccurred(tr("No instance selected."));
        return;
    }
    
    auto instance = resolveInstance(m_selectedInstanceId);
    if (!instance) {
        emit errorOccurred(tr("The selected instance could not be found."));
        return;
    }
    
    // Open export dialog via InstanceWindow export page
    APPLICATION->showInstanceWindow(instance, "export");
}

void InstanceListViewModel::manageSelectedBackups()
{
    if (m_selectedInstanceId.isEmpty()) {
        emit errorOccurred(tr("No instance selected."));
        return;
    }
    
    auto instance = resolveInstance(m_selectedInstanceId);
    if (!instance) {
        emit errorOccurred(tr("The selected instance could not be found."));
        return;
    }
    
    // Open backup manager via InstanceWindow backups page
    APPLICATION->showInstanceWindow(instance, "backups");
}

void InstanceListViewModel::createSelectedShortcut()
{
    if (m_selectedInstanceId.isEmpty()) {
        emit errorOccurred(tr("No instance selected."));
        return;
    }
    
    auto instance = resolveInstance(m_selectedInstanceId);
    if (!instance) {
        emit errorOccurred(tr("The selected instance could not be found."));
        return;
    }
    
    // Create desktop shortcut
    ShortcutUtils::Shortcut shortcut;
    shortcut.instance = instance.get();
    shortcut.name = instance->name();
    shortcut.iconKey = instance->iconKey();
    
    if (ShortcutUtils::createInstanceShortcutOnDesktop(shortcut)) {
        qDebug() << "Shortcut created successfully for" << instance->name();
    } else {
        emit errorOccurred(tr("Failed to create shortcut."));
    }
}

QString InstanceListViewModel::selectedInstanceName() const
{
    if (m_selectedInstanceId.isEmpty()) {
        return QString();
    }
    
    int idx = m_instanceIds.indexOf(m_selectedInstanceId);
    if (idx >= 0 && idx < m_instanceNames.size()) {
        return m_instanceNames[idx];
    }
    return QString();
}
