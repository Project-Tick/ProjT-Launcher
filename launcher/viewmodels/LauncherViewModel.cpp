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
#include <QFileDialog>
#include <QWidget>

LauncherViewModel::LauncherViewModel(QObject* parent) : QObject(parent)
{
    m_displayName = BuildConfig.LAUNCHER_DISPLAYNAME;
    m_versionString = BuildConfig.printableVersionString();
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

void LauncherViewModel::openHelp()
{
    DesktopServices::openUrl(QUrl("https://proj-t.com/help"));
}

void LauncherViewModel::checkUpdates()
{
    qDebug() << "Update check requested (Not implemented yet)";
    // TODO: Implement update check trigger
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
