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

#pragma once

#include <QObject>
#include <QString>

class LauncherViewModel : public QObject {
    Q_OBJECT
   public:
    enum class Page
    {
        Instances = 0,
        News,
        Settings,
        About,
        Logs
    };
    Q_ENUM(Page)

    Q_PROPERTY(QString displayName READ displayName NOTIFY displayNameChanged)
    Q_PROPERTY(QString versionString READ versionString NOTIFY versionStringChanged)
    Q_PROPERTY(QString gitRef READ gitRef NOTIFY gitRefChanged)
    Q_PROPERTY(QString gitCommit READ gitCommit NOTIFY gitCommitChanged)
    Q_PROPERTY(QString aboutHtml READ aboutHtml NOTIFY aboutHtmlChanged)
    Q_PROPERTY(bool busy READ isBusy WRITE setBusy NOTIFY busyChanged)
    Q_PROPERTY(Page currentPage READ currentPage WRITE setCurrentPage NOTIFY currentPageChanged)

   public:
    explicit LauncherViewModel(QObject* parent = nullptr);

    QString displayName() const;
    QString versionString() const;
    QString gitRef() const;
    QString gitCommit() const;
    QString aboutHtml() const;
    bool isBusy() const;
    Page currentPage() const;

    void setDisplayName(const QString& name);
    void setVersionString(const QString& version);
    void setGitRef(const QString& ref);
    void setGitCommit(const QString& commit);
    void setAboutHtml(const QString& html);
    void setBusy(bool busy);
    void setCurrentPage(Page page);

    static QString pageToString(Page page);
    static Page stringToPage(const QString& route);

   signals:
    void displayNameChanged();
    void versionStringChanged();
    void gitRefChanged();
    void gitCommitChanged();
    void aboutHtmlChanged();
    void busyChanged();
    void currentPageChanged();

   private:
    QString m_displayName;
    QString m_versionString;
    QString m_gitRef;
    QString m_gitCommit;
    QString m_aboutHtml;
    bool m_busy = false;
    Page m_currentPage = Page::Instances;
};
