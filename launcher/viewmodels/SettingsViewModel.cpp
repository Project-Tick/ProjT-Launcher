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

#include "SettingsViewModel.h"

#include <QCoreApplication>
#include <QMessageBox>

#include "Application.h"
#include "BaseInstance.h"
#include "InstanceList.h"
#include "Json.h"
#include "JavaCommon.h"
#include "java/JavaInstall.h"
#include "java/JavaInstallList.h"
#include "java/JavaUtils.h"
#include "icons/IconList.h"
#include "settings/SettingsObject.h"
#include "ui/dialogs/CustomMessageBox.h"
#include "ui/dialogs/VersionSelectDialog.h"

SettingsViewModel::SettingsViewModel(QObject* parent) : QObject(parent) {}

std::shared_ptr<SettingsObject> SettingsViewModel::settingsForInstance(const QString& instanceId) const
{
    if (instanceId.isEmpty()) {
        return APPLICATION->settings();
    }
    auto list = APPLICATION ? APPLICATION->instances() : nullptr;
    if (!list) {
        return {};
    }
    auto inst = list->getInstanceById(instanceId);
    return inst ? inst->settings() : std::shared_ptr<SettingsObject>();
}

QMap<QString, QVariant> SettingsViewModel::loadEnv(const QString& instanceId) const
{
    auto settings = settingsForInstance(instanceId);
    if (!settings) {
        return {};
    }
    return Json::toMap(settings->get("Env").toString());
}

void SettingsViewModel::storeEnv(const QString& instanceId, bool overrideEnv, const QMap<QString, QVariant>& vars)
{
    auto settings = settingsForInstance(instanceId);
    if (!settings) {
        return;
    }
    SettingsObject::Lock lock(settings);
    settings->set("OverrideEnv", overrideEnv);
    settings->set("Env", Json::fromMap(vars));
    m_overrideEnv = overrideEnv;
    emit overrideEnvChanged();
    emit envVarsChanged();
}

QString SettingsViewModel::instanceId() const
{
    return m_instanceId;
}

QString SettingsViewModel::currentCategory() const
{
    return m_currentCategory;
}

void SettingsViewModel::setCategoryList(const QStringList& categories)
{
    if (m_categoryList == categories) {
        return;
    }
    m_categoryList = categories;
    emit categoryListChanged();
}

bool SettingsViewModel::isBusy() const
{
    return m_busy;
}

QString SettingsViewModel::javaPath() const
{
    return m_javaPath;
}

bool SettingsViewModel::overrideJavaLocation() const
{
    return m_overrideJavaLocation;
}

bool SettingsViewModel::saveBusy() const
{
    return m_saveBusy;
}

QString SettingsViewModel::lastErrorMessage() const
{
    return m_lastErrorMessage;
}

QString SettingsViewModel::preLaunchCommand() const
{
    return m_preLaunchCommand;
}

QString SettingsViewModel::postExitCommand() const
{
    return m_postExitCommand;
}

void SettingsViewModel::setInstanceId(const QString& id)
{
    if (m_instanceId == id) {
        return;
    }
    m_instanceId = id;
    emit instanceIdChanged();
}

void SettingsViewModel::setCurrentCategory(const QString& category)
{
    if (m_currentCategory == category) {
        return;
    }
    m_currentCategory = category;
    emit currentCategoryChanged();
}

void SettingsViewModel::setBusy(bool busy)
{
    if (m_busy == busy) {
        return;
    }
    m_busy = busy;
    emit busyChanged();
}

void SettingsViewModel::setLoaderTypeProperty(const QString& type)
{
    setLoaderType(m_instanceId, type);
}

void SettingsViewModel::setLoaderVersionProperty(const QString& version)
{
    setLoaderVersion(m_instanceId, version);
}

void SettingsViewModel::setJavaPath(const QString& path)
{
    if (m_javaPath == path) {
        return;
    }
    m_javaPath = path;
    emit javaPathChanged();
}

void SettingsViewModel::setOverrideJavaLocation(bool value)
{
    if (m_overrideJavaLocation == value) {
        return;
    }
    m_overrideJavaLocation = value;
    emit overrideJavaLocationChanged();
}

void SettingsViewModel::setSaveBusy(bool busy)
{
    if (m_saveBusy == busy) {
        return;
    }
    m_saveBusy = busy;
    emit saveBusyChanged();
}

void SettingsViewModel::setLastErrorMessage(const QString& message)
{
    if (m_lastErrorMessage == message) {
        return;
    }
    m_lastErrorMessage = message;
    emit lastErrorMessageChanged();
}

void SettingsViewModel::notifySettingsLoaded()
{
    emit settingsLoaded();
}

void SettingsViewModel::notifySettingsChanged()
{
    emit settingsChanged();
}

void SettingsViewModel::notifySaveRequested()
{
    emit saveRequested();
}

void SettingsViewModel::refresh()
{
    loadCategory(m_currentCategory.isEmpty() ? QStringLiteral("java") : m_currentCategory);
}

void SettingsViewModel::loadCategory(const QString& category)
{
    setCurrentCategory(category);
    loadCurrentSettings();
    notifySettingsLoaded();
}

void SettingsViewModel::applyChanges()
{
    m_busyReason = tr("Applying settings");
    emit started(m_busyReason);
    setBusy(true);
    bool ok = true;
    if (m_applyHook) {
        ok = m_applyHook();
    }
    if (ok) {
        notifySettingsChanged();
        emit finished();
    } else {
        emit errorOccurred(tr("Failed to apply settings."));
    }
    setBusy(false);
}

void SettingsViewModel::resetChanges()
{
    m_busyReason = tr("Resetting settings");
    emit started(m_busyReason);
    setBusy(true);
    if (m_resetHook) {
        m_resetHook();
    } else {
        resetToDefaultsForCurrentCategory();
    }
    emit finished();
    setBusy(false);
}

void SettingsViewModel::saveAll()
{
    auto instances = APPLICATION ? APPLICATION->instances() : nullptr;
    if (!instances) {
        return;
    }
    auto inst = instances->getInstanceById(m_instanceId);
    if (!inst) {
        return;
    }

    setSaveBusy(true);
    auto settings = inst->settings();
    if (settings) {
        settings->set("JavaPath", m_javaPath);
        settings->set("OverrideJavaLocation", m_overrideJavaLocation);
        settings->set("PreLaunchCommand", m_preLaunchCommand);
        settings->set("PostExitCommand", m_postExitCommand);
    }
    inst->saveNow();
    setSaveBusy(false);
    notifySettingsChanged();
}

void SettingsViewModel::resetToDefaultsForCurrentCategory()
{
    resetJavaCategory();
}

QStringList SettingsViewModel::environmentKeys(const QString& instanceId) const
{
    return loadEnv(instanceId).keys();
}

QString SettingsViewModel::environmentValue(const QString& instanceId, const QString& key) const
{
    return loadEnv(instanceId).value(key).toString();
}

void SettingsViewModel::setEnvironmentVar(const QString& instanceId, const QString& key, const QString& value)
{
    if (key.isEmpty()) {
        return;
    }
    auto env = loadEnv(instanceId);
    env.insert(key, value);
    storeEnv(instanceId, true, env);
}

void SettingsViewModel::removeEnvironmentVar(const QString& instanceId, const QString& key)
{
    if (key.isEmpty()) {
        return;
    }
    auto env = loadEnv(instanceId);
    env.remove(key);
    storeEnv(instanceId, true, env);
}

void SettingsViewModel::clearEnvironmentVars(const QString& instanceId)
{
    storeEnv(instanceId, false, {});
}

void SettingsViewModel::setEnvironmentVars(const QString& instanceId, bool overrideEnv, const QMap<QString, QVariant>& vars)
{
    storeEnv(instanceId, overrideEnv, vars);
}

void SettingsViewModel::setMemorySettings(const QString& instanceId, int minMem, int maxMem)
{
    auto settings = settingsForInstance(instanceId);
    if (!settings) {
        return;
    }
    if (minMem > maxMem) {
        std::swap(minMem, maxMem);
    }
    m_minMemory = minMem;
    m_maxMemory = maxMem;
    emit memoryChanged();
    SettingsObject::Lock lock(settings);
    settings->set("OverrideMemory", true);
    if (!m_overrideMemory) {
        m_overrideMemory = true;
        emit overrideMemoryChanged();
    }
    settings->set("MinMemAlloc", m_minMemory);
    settings->set("MaxMemAlloc", m_maxMemory);
}

void SettingsViewModel::setJVMArguments(const QString& instanceId, const QString& args)
{
    auto settings = settingsForInstance(instanceId);
    if (!settings) {
        return;
    }
    m_jvmArgs = args;
    emit jvmArgsChanged();
    SettingsObject::Lock lock(settings);
    settings->set("OverrideJavaArgs", true);
    settings->set("JvmArgs", m_jvmArgs);
}

void SettingsViewModel::setJavaPath(const QString& instanceId, const QString& path)
{
    setJavaPath(path);
    auto settings = settingsForInstance(instanceId);
    if (!settings) {
        return;
    }
    SettingsObject::Lock lock(settings);
    settings->set("OverrideJavaLocation", true);
    settings->set("JavaPath", path);
}

void SettingsViewModel::setOverrideJavaLocation(const QString& instanceId, bool value)
{
    setOverrideJavaLocation(value);
    auto settings = settingsForInstance(instanceId);
    if (!settings) {
        return;
    }
    SettingsObject::Lock lock(settings);
    settings->set("OverrideJavaLocation", value);
}

void SettingsViewModel::setJavaArgs(const QString& instanceId, const QString& args)
{
    setJVMArguments(instanceId, args);
}

void SettingsViewModel::setJavaMemory(const QString& instanceId, int minMem, int maxMem)
{
    setMemorySettings(instanceId, minMem, maxMem);
}

void SettingsViewModel::autoDetectJava(const QString& instanceId, QWidget* parent)
{
    if (JavaUtils::getJavaCheckPath().isEmpty()) {
        JavaCommon::javaCheckNotFound(parent);
        return;
    }

    VersionSelectDialog versionDialog(APPLICATION->javalist().get(), QCoreApplication::translate("SettingsViewModel", "Select a Java version"), parent,
                                      true);
    versionDialog.setResizeOn(2);
    versionDialog.exec();

    if (versionDialog.result() == QDialog::Accepted && versionDialog.selectedVersion()) {
        JavaInstallPtr java = std::dynamic_pointer_cast<JavaInstall>(versionDialog.selectedVersion());
        if (!java) {
            return;
        }
        setJavaPath(instanceId, java->path);

        if (!java->is_64bit && m_maxMemory > 2048) {
            CustomMessageBox::selectable(parent, QCoreApplication::translate("SettingsViewModel", "Confirm Selection"),
                                         QCoreApplication::translate("SettingsViewModel",
                                                                     "You selected a 32-bit version of Java.\n"
                                                                     "This installation does not support more than 2048MiB of RAM.\n"
                                                                     "Please make sure that the maximum memory value is lower."),
                                         QMessageBox::Warning, QMessageBox::Ok, QMessageBox::Ok)
                ->exec();
        }
    }
}

void SettingsViewModel::setLoaderType(const QString& instanceId, const QString& type)
{
    setLoaderPreferences(instanceId, QStringList{ type }, true);
}

void SettingsViewModel::setLoaderVersion(const QString& instanceId, const QString& version)
{
    auto settings = settingsForInstance(instanceId);
    if (!settings) {
        return;
    }
    SettingsObject::Lock lock(settings);
    settings->set("PreferredLoaderVersion", version);
    emit loaderSettingsChanged();
}

void SettingsViewModel::refreshLoaderVersions(const QString& instanceId)
{
    Q_UNUSED(instanceId);
    emit loaderSettingsChanged();
}

void SettingsViewModel::setLoaderPreferences(const QString& instanceId, const QStringList& loaders, bool overrideLoaders)
{
    auto settings = settingsForInstance(instanceId);
    if (!settings) {
        return;
    }
    SettingsObject::Lock lock(settings);
    settings->set("OverrideModDownloadLoaders", overrideLoaders);
    settings->set("ModDownloadLoaders", loaders);
    m_overrideLoader = overrideLoaders;
    emit overrideLoaderChanged();
    emit loaderSettingsChanged();
}

void SettingsViewModel::setOverrideMemory(const QString& instanceId, bool value)
{
    auto settings = settingsForInstance(instanceId);
    if (!settings) {
        return;
    }
    SettingsObject::Lock lock(settings);
    settings->set("OverrideMemory", value);
    if (m_overrideMemory != value) {
        m_overrideMemory = value;
        emit overrideMemoryChanged();
    }
}

void SettingsViewModel::setOverrideLoader(const QString& instanceId, bool value)
{
    setLoaderPreferences(instanceId, {}, value);
}

void SettingsViewModel::setGameArgs(const QString& instanceId, const QString& args)
{
    auto settings = settingsForInstance(instanceId);
    if (!settings) {
        return;
    }
    m_gameArgs = args;
    SettingsObject::Lock lock(settings);
    settings->set("WrapperCommand", args);
    emit gameSettingsChanged();
}

void SettingsViewModel::setFullscreen(const QString& instanceId, bool enabled)
{
    auto settings = settingsForInstance(instanceId);
    if (!settings) {
        return;
    }
    m_fullscreen = enabled;
    SettingsObject::Lock lock(settings);
    settings->set("LaunchMaximized", enabled);
    emit gameSettingsChanged();
}

void SettingsViewModel::setResolution(const QString& instanceId, int width, int height)
{
    auto settings = settingsForInstance(instanceId);
    if (!settings) {
        return;
    }
    m_resolutionWidth = width;
    m_resolutionHeight = height;
    SettingsObject::Lock lock(settings);
    settings->set("MinecraftWinWidth", width);
    settings->set("MinecraftWinHeight", height);
    emit gameSettingsChanged();
}

void SettingsViewModel::setOverrideGameDir(const QString& instanceId, bool value)
{
    auto settings = settingsForInstance(instanceId);
    if (!settings) {
        return;
    }
    m_overrideGameDir = value;
    SettingsObject::Lock lock(settings);
    settings->set("OverrideGameDir", value);
    emit gameSettingsChanged();
}

void SettingsViewModel::setCustomGameDir(const QString& instanceId, const QString& path)
{
    auto settings = settingsForInstance(instanceId);
    if (!settings) {
        return;
    }
    m_customGameDir = path;
    SettingsObject::Lock lock(settings);
    settings->set("GameDir", path);
    emit gameSettingsChanged();
}

void SettingsViewModel::setNotes(const QString& instanceId, const QString& notes)
{
    auto instances = APPLICATION ? APPLICATION->instances() : nullptr;
    if (!instances) {
        return;
    }
    auto inst = instances->getInstanceById(instanceId);
    if (!inst) {
        return;
    }
    m_notes = notes;
    inst->setNotes(notes);
    emit notesChanged();
}

void SettingsViewModel::setIconKey(const QString& instanceId, const QString& iconKey)
{
    auto instances = APPLICATION ? APPLICATION->instances() : nullptr;
    if (!instances) {
        return;
    }
    auto inst = instances->getInstanceById(instanceId);
    if (!inst) {
        return;
    }
    m_iconKey = iconKey;
    inst->setIconKey(iconKey);
    emit iconChanged();
}

void SettingsViewModel::setOverrideEnv(const QString& instanceId, bool value)
{
    auto settings = settingsForInstance(instanceId);
    if (!settings) {
        return;
    }
    SettingsObject::Lock lock(settings);
    settings->set("OverrideEnv", value);
    if (m_overrideEnv != value) {
        m_overrideEnv = value;
        emit overrideEnvChanged();
    }
}

void SettingsViewModel::setPreLaunchCommand(const QString& instanceId, const QString& cmd)
{
    Q_UNUSED(instanceId);
    if (m_preLaunchCommand == cmd) {
        return;
    }
    m_preLaunchCommand = cmd;
    emit customCommandsChanged();
}

void SettingsViewModel::setPostExitCommand(const QString& instanceId, const QString& cmd)
{
    Q_UNUSED(instanceId);
    if (m_postExitCommand == cmd) {
        return;
    }
    m_postExitCommand = cmd;
    emit customCommandsChanged();
}

void SettingsViewModel::loadCurrentSettings()
{
    auto instances = APPLICATION ? APPLICATION->instances() : nullptr;
    if (!instances) {
        return;
    }
    auto inst = instances->getInstanceById(m_instanceId);
    if (!inst) {
        return;
    }
    auto settings = inst->settings();
    if (!settings) {
        return;
    }
    setJavaPath(settings->get("JavaPath").toString());
    setOverrideJavaLocation(settings->get("OverrideJavaLocation").toBool());
    m_preLaunchCommand = settings->get("PreLaunchCommand").toString();
    m_postExitCommand = settings->get("PostExitCommand").toString();
    m_minMemory = settings->get("MinMemAlloc").toInt();
    m_maxMemory = settings->get("MaxMemAlloc").toInt();
    m_jvmArgs = settings->get("JvmArgs").toString();
    m_overrideMemory = settings->get("OverrideMemory").toBool();
    m_overrideLoader = settings->get("OverrideModDownloadLoaders").toBool();
    m_overrideEnv = settings->get("OverrideEnv").toBool();
    m_loaderType = settings->get("ModDownloadLoaders").toStringList().value(0);
    m_availableLoaderTypes = settings->get("ModDownloadLoaders").toStringList();
    if (m_availableLoaderTypes.isEmpty()) {
        m_availableLoaderTypes = QStringList{ "Fabric", "Forge", "Quilt", "NeoForge" };
    }
    m_loaderVersion = settings->get("PreferredLoaderVersion").toString();
    m_availableLoaderVersions.clear();
    m_gameArgs = settings->get("WrapperCommand").toString();
    m_fullscreen = settings->get("LaunchMaximized").toBool();
    m_resolutionWidth = settings->get("MinecraftWinWidth").toInt();
    m_resolutionHeight = settings->get("MinecraftWinHeight").toInt();
    m_overrideGameDir = settings->get("OverrideGameDir").toBool();
    m_customGameDir = settings->get("GameDir").toString();
    m_notes = inst->notes();
    m_iconKey = inst->iconKey();
    m_availableIcons.clear();
    if (auto iconList = APPLICATION->icons()) {
        const int rows = iconList->rowCount();
        for (int row = 0; row < rows; ++row) {
            const auto idx = iconList->index(row, 0);
            m_availableIcons.append(iconList->data(idx, Qt::UserRole).toString());
        }
    }
    emit customCommandsChanged();
    emit memoryChanged();
    emit jvmArgsChanged();
    emit overrideMemoryChanged();
    emit overrideLoaderChanged();
    emit overrideEnvChanged();
    emit loaderSettingsChanged();
    emit gameSettingsChanged();
    emit notesChanged();
    emit iconChanged();
}

void SettingsViewModel::resetJavaCategory()
{
    auto instances = APPLICATION ? APPLICATION->instances() : nullptr;
    if (!instances) {
        return;
    }
    auto inst = instances->getInstanceById(m_instanceId);
    if (!inst) {
        return;
    }
    auto settings = inst->settings();
    if (!settings) {
        return;
    }
    settings->reset("JavaPath");
    settings->reset("OverrideJavaLocation");
    loadCurrentSettings();
}

void SettingsViewModel::setApplyHook(std::function<bool()> hook)
{
    m_applyHook = std::move(hook);
}

void SettingsViewModel::setResetHook(std::function<void()> hook)
{
    m_resetHook = std::move(hook);
}
