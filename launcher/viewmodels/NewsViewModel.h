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
#include "QObjectPtr.h"

class NewsChecker;
class NewsViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString currentContent READ currentContent NOTIFY currentContentChanged)
    Q_PROPERTY(bool busy READ isBusy NOTIFY busyChanged)
    Q_PROPERTY(QDateTime lastUpdated READ lastUpdated NOTIFY lastUpdatedChanged)
    Q_PROPERTY(QString currentTitle READ currentTitle NOTIFY currentTitleChanged)
    Q_PROPERTY(QString currentLink READ currentLink NOTIFY currentLinkChanged)
    Q_PROPERTY(QString currentArticleHtml READ currentContent NOTIFY currentContentChanged)
    Q_PROPERTY(QStringList titles READ titles NOTIFY entriesChanged)
    Q_PROPERTY(QStringList links READ links NOTIFY entriesChanged)
    Q_PROPERTY(QStringList htmlBodies READ htmlBodies NOTIFY entriesChanged)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)

   public:
    explicit NewsViewModel(QObject* parent = nullptr);

    QString currentContent() const;
    QString currentTitle() const;
    QString currentLink() const;
    bool isBusy() const;
    QDateTime lastUpdated() const;
    QStringList titles() const;
    QStringList links() const;
    QStringList htmlBodies() const;
    int currentIndex() const;

    QList<NewsEntryPtr> entries() const;
    void setEntries(const QList<NewsEntryPtr>& entries);
    void selectArticle(const QString& title);
    void setCurrentIndex(int index);
    NewsEntryPtr entryForTitle(const QString& title) const;

    void setBusy(bool busy);
    void setLastUpdated(const QDateTime& timestamp);

   public slots:
    void refresh();
    void selectByIndex(int index);
    void openCurrentLink();

   signals:
    void started();
    void finished();
    void errorOccurred(const QString& message);
    void currentContentChanged();
    void currentTitleChanged();
    void currentLinkChanged();
    void busyChanged();
    void lastUpdatedChanged();
    void newsUpdated();
    void entriesChanged();
    void currentIndexChanged();
    void openLinkRequested(const QString& link);

   private:
    void updateCurrentFromEntry(const NewsEntryPtr& entry);
    void handleNewsLoaded();
    void handleNewsLoadFailed(const QString& error);
    void startRefresh();

    QList<NewsEntryPtr> m_entries;
    QString m_currentTitle;
    QString m_currentContent;
    QString m_currentLink;
    bool m_busy = false;
    QDateTime m_lastUpdated;
    int m_currentIndex = -1;
    unique_qobject_ptr<NewsChecker> m_newsChecker;
};
