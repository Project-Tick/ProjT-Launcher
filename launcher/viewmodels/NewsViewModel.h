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

#include <QDateTime>
#include <QObject>

#include "news/NewsEntry.h"

class NewsViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString currentContent READ currentContent NOTIFY currentContentChanged)
    Q_PROPERTY(bool busy READ isBusy NOTIFY busyChanged)
    Q_PROPERTY(QDateTime lastUpdated READ lastUpdated NOTIFY lastUpdatedChanged)

   public:
    explicit NewsViewModel(QObject* parent = nullptr);

    QString currentContent() const;
    QString currentTitle() const;
    QString currentLink() const;
    bool isBusy() const;
    QDateTime lastUpdated() const;

    QList<NewsEntryPtr> entries() const;
    void setEntries(const QList<NewsEntryPtr>& entries);
    void selectArticle(const QString& title);
    NewsEntryPtr entryForTitle(const QString& title) const;

    void setBusy(bool busy);
    void setLastUpdated(const QDateTime& timestamp);

   public slots:
    void requestRefresh();

   signals:
    void currentContentChanged();
    void busyChanged();
    void lastUpdatedChanged();
    void newsUpdated();
    void refreshRequested();

   private:
    void updateCurrentFromEntry(const NewsEntryPtr& entry);

    QList<NewsEntryPtr> m_entries;
    QString m_currentTitle;
    QString m_currentContent;
    QString m_currentLink;
    bool m_busy = false;
    QDateTime m_lastUpdated;
};
