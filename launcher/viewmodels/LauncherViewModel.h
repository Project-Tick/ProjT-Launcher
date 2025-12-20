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

#pragma once

#include <QObject>
#include <QString>

class LauncherViewModel : public QObject {
    Q_OBJECT
   public:
    enum class Page { Instances = 0, News, Settings, About, Logs };
    Q_ENUM(Page)

    Q_PROPERTY(QString displayName READ displayName NOTIFY displayNameChanged)
    Q_PROPERTY(QString versionString READ versionString NOTIFY versionStringChanged)
    Q_PROPERTY(QString gitRef READ gitRef NOTIFY gitRefChanged)
    Q_PROPERTY(QString gitCommit READ gitCommit NOTIFY gitCommitChanged)
    Q_PROPERTY(QString aboutHtml READ aboutHtml NOTIFY aboutHtmlChanged)
    Q_PROPERTY(bool busy READ isBusy WRITE setBusy NOTIFY busyChanged)
    Q_PROPERTY(Page currentPage READ currentPage WRITE setCurrentPage NOTIFY currentPageChanged)
    Q_PROPERTY(bool hasUpdate READ hasUpdate NOTIFY updateStatusChanged)
    Q_PROPERTY(QString updateVersion READ updateVersion NOTIFY updateStatusChanged)

   public:
    explicit LauncherViewModel(QObject* parent = nullptr);

    QString displayName() const;
    QString versionString() const;
    QString gitRef() const;
    QString gitCommit() const;
    QString aboutHtml() const;
    bool isBusy() const;
    Page currentPage() const;
    bool hasUpdate() const;
    QString updateVersion() const;

    void setDisplayName(const QString& name);
    void setVersionString(const QString& version);
    void setGitRef(const QString& ref);
    void setGitCommit(const QString& commit);
    void setAboutHtml(const QString& html);
    void setBusy(bool busy);
    void setCurrentPage(Page page);

    static QString pageToString(Page page);
    static Page stringToPage(const QString& route);

    Q_INVOKABLE void openDataFolder();
    Q_INVOKABLE void openHelp();
    Q_INVOKABLE void checkUpdates();

    // Folder actions
    Q_INVOKABLE void openLauncherFolder();
    Q_INVOKABLE void openInstancesFolder();
    Q_INVOKABLE void openModsFolder();
    Q_INVOKABLE void openSkinsFolder();

    // Dialog actions
    Q_INVOKABLE void openAccountsManager();

    Q_INVOKABLE QString browseForFile(const QString& title, const QString& filter);
    Q_INVOKABLE QStringList browseForFiles(const QString& title, const QString& filter);
    Q_INVOKABLE QString browseForDirectory(const QString& title);
    Q_INVOKABLE QString browseForFolder(const QString& title);
    Q_INVOKABLE QString browseForSave(const QString& title, const QString& filter);

    // Application control
    Q_INVOKABLE void quit();

   signals:
    void displayNameChanged();
    void versionStringChanged();
    void gitRefChanged();
    void gitCommitChanged();
    void aboutHtmlChanged();
    void busyChanged();
    void currentPageChanged();
    void updateStatusChanged();

   private:
    QString m_displayName;
    QString m_versionString;
    QString m_gitRef;
    QString m_gitCommit;
    QString m_aboutHtml;
    bool m_busy = false;
    Page m_currentPage = Page::Instances;
    bool m_hasUpdate = false;
    QString m_updateVersion;
};
