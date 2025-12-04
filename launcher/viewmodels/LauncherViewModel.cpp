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

#include "LauncherViewModel.h"

#include "BuildConfig.h"
#include "Application.h"
#include "DesktopServices.h"
#include <QUrl>
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QWidget>

LauncherViewModel::LauncherViewModel(QObject* parent) : QObject(parent)
{
    m_displayName = BuildConfig.LAUNCHER_DISPLAYNAME;
    m_versionString = BuildConfig.printableVersionString();
    m_gitRef = BuildConfig.GIT_REFSPEC;
    m_gitCommit = BuildConfig.GIT_COMMIT;
    
    // Build About HTML
    m_aboutHtml = QObject::tr(
        "<b>%1</b><br/>"
        "Version: %2<br/>"
        "Git: %3<br/><br/>"
        "%4"
    ).arg(
        m_displayName,
        m_versionString,
        BuildConfig.LAUNCHER_GIT,
        BuildConfig.LAUNCHER_COPYRIGHT
    );
}

QString LauncherViewModel::displayName() const
{
    return m_displayName;
}

QString LauncherViewModel::versionString() const
{
    return m_versionString;
}

QString LauncherViewModel::gitRef() const
{
    return m_gitRef;
}

QString LauncherViewModel::gitCommit() const
{
    return m_gitCommit;
}

QString LauncherViewModel::aboutHtml() const
{
    return m_aboutHtml;
}

bool LauncherViewModel::isBusy() const
{
    return m_busy;
}

LauncherViewModel::Page LauncherViewModel::currentPage() const
{
    return m_currentPage;
}

bool LauncherViewModel::hasUpdate() const
{
    return m_hasUpdate;
}

QString LauncherViewModel::updateVersion() const
{
    return m_updateVersion;
}

void LauncherViewModel::setDisplayName(const QString& name)
{
    if (m_displayName == name) {
        return;
    }
    m_displayName = name;
    emit displayNameChanged();
}

void LauncherViewModel::setVersionString(const QString& version)
{
    if (m_versionString == version) {
        return;
    }
    m_versionString = version;
    emit versionStringChanged();
}

void LauncherViewModel::setBusy(bool busy)
{
    if (m_busy == busy) {
        return;
    }
    m_busy = busy;
    emit busyChanged();
}

void LauncherViewModel::setGitRef(const QString& ref)
{
    if (m_gitRef == ref) {
        return;
    }
    m_gitRef = ref;
    emit gitRefChanged();
}

void LauncherViewModel::setGitCommit(const QString& commit)
{
    if (m_gitCommit == commit) {
        return;
    }
    m_gitCommit = commit;
    emit gitCommitChanged();
}

void LauncherViewModel::setAboutHtml(const QString& html)
{
    if (m_aboutHtml == html) {
        return;
    }
    m_aboutHtml = html;
    emit aboutHtmlChanged();
}

void LauncherViewModel::setCurrentPage(Page page)
{
    if (m_currentPage == page) {
        return;
    }
    m_currentPage = page;
    emit currentPageChanged();
}

QString LauncherViewModel::pageToString(Page page)
{
    switch (page) {
    case Page::News:
        return QStringLiteral("news");
    case Page::Settings:
        return QStringLiteral("settings");
    case Page::About:
        return QStringLiteral("about");
    case Page::Logs:
        return QStringLiteral("logs");
    case Page::Instances:
    default:
        return QStringLiteral("instances");
    }
}

LauncherViewModel::Page LauncherViewModel::stringToPage(const QString& route)
{
    const auto lower = route.trimmed().toLower();
    if (lower == QStringLiteral("news")) {
        return Page::News;
    }
    if (lower == QStringLiteral("settings")) {
        return Page::Settings;
    }
    if (lower == QStringLiteral("about")) {
        return Page::About;
    }
    if (lower == QStringLiteral("logs")) {
        return Page::Logs;
    }
    return Page::Instances;
}

void LauncherViewModel::openDataFolder()
{
    DesktopServices::openPath(APPLICATION->dataRoot());
}

void LauncherViewModel::openLauncherFolder()
{
    DesktopServices::openPath(APPLICATION->dataRoot());
}

void LauncherViewModel::openInstancesFolder()
{
    // Instances folder is typically at dataRoot/instances
    QString instancesPath = QDir(APPLICATION->dataRoot()).absoluteFilePath("instances");
    QDir instancesDir(instancesPath);
    if (!instancesDir.exists()) {
        // Fallback to dataRoot if instances subfolder doesn't exist
        DesktopServices::openPath(APPLICATION->dataRoot());
    } else {
        DesktopServices::openPath(instancesPath);
    }
}

void LauncherViewModel::openModsFolder()
{
    // Central mods folder (shared mods)
    QString modsPath = QDir(APPLICATION->dataRoot()).absoluteFilePath("mods");
    QDir modsDir(modsPath);
    if (!modsDir.exists()) {
        modsDir.mkpath(".");
    }
    DesktopServices::openPath(modsPath);
}

void LauncherViewModel::openSkinsFolder()
{
    // Skins folder
    QString skinsPath = QDir(APPLICATION->dataRoot()).absoluteFilePath("skins");
    QDir skinsDir(skinsPath);
    if (!skinsDir.exists()) {
        skinsDir.mkpath(".");
    }
    DesktopServices::openPath(skinsPath);
}

void LauncherViewModel::openHelp()
{
    DesktopServices::openUrl(QUrl("https://proj-t.com/help"));
}

void LauncherViewModel::checkUpdates()
{
    qDebug() << "[LauncherViewModel] Update check requested";
    // Trigger update check through Application
    APPLICATION->triggerUpdateCheck();
}

void LauncherViewModel::openAccountsManager()
{
    // Show accounts management dialog via Application
    APPLICATION->ShowGlobalSettings(nullptr, "accounts");
}

QString LauncherViewModel::browseForFile(const QString& title, const QString& filter)
{
    // We need a parent widget for the dialog to be modal
    QWidget* parent = nullptr;
    if (QApplication::activeWindow()) {
        parent = QApplication::activeWindow();
    }
    return QFileDialog::getOpenFileName(parent, title, QString(), filter);
}

QString LauncherViewModel::browseForDirectory(const QString& title)
{
    QWidget* parent = nullptr;
    if (QApplication::activeWindow()) {
        parent = QApplication::activeWindow();
    }
    return QFileDialog::getExistingDirectory(parent, title);
}

QString LauncherViewModel::browseForSave(const QString& title, const QString& filter)
{
    QWidget* parent = nullptr;
    if (QApplication::activeWindow()) {
        parent = QApplication::activeWindow();
    }
    return QFileDialog::getSaveFileName(parent, title, QString(), filter);
}
