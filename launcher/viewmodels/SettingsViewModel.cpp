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

#include "Application.h"
#include "BaseInstance.h"
#include "InstanceList.h"

SettingsViewModel::SettingsViewModel(QObject* parent) : QObject(parent) {}

QString SettingsViewModel::instanceId() const
{
    return m_instanceId;
}

QString SettingsViewModel::currentCategory() const
{
    return m_currentCategory;
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
    loadCurrentSettings();
    notifySettingsLoaded();
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
    }
    inst->saveNow();
    setSaveBusy(false);
    notifySettingsChanged();
}

void SettingsViewModel::resetToDefaultsForCurrentCategory()
{
    resetJavaCategory();
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
