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

#include "NewsViewModel.h"

NewsViewModel::NewsViewModel(QObject* parent) : QObject(parent) {}

QString NewsViewModel::currentContent() const
{
    return m_currentContent;
}

QString NewsViewModel::currentTitle() const
{
    return m_currentTitle;
}

QString NewsViewModel::currentLink() const
{
    return m_currentLink;
}

bool NewsViewModel::isBusy() const
{
    return m_busy;
}

QDateTime NewsViewModel::lastUpdated() const
{
    return m_lastUpdated;
}

QList<NewsEntryPtr> NewsViewModel::entries() const
{
    return m_entries;
}

void NewsViewModel::setEntries(const QList<NewsEntryPtr>& entries)
{
    m_entries = entries;
    if (!m_entries.isEmpty()) {
        updateCurrentFromEntry(m_entries.front());
    } else {
        updateCurrentFromEntry(nullptr);
    }
    emit newsUpdated();
}

void NewsViewModel::selectArticle(const QString& title)
{
    if (title.isEmpty()) {
        return;
    }
    const auto entry = entryForTitle(title);
    if (!entry) {
        return;
    }
    updateCurrentFromEntry(entry);
}

NewsEntryPtr NewsViewModel::entryForTitle(const QString& title) const
{
    for (const auto& entry : m_entries) {
        if (entry && entry->title == title) {
            return entry;
        }
    }
    return {};
}

void NewsViewModel::setBusy(bool busy)
{
    if (m_busy == busy) {
        return;
    }
    m_busy = busy;
    emit busyChanged();
}

void NewsViewModel::setLastUpdated(const QDateTime& timestamp)
{
    if (m_lastUpdated == timestamp) {
        return;
    }
    m_lastUpdated = timestamp;
    emit lastUpdatedChanged();
}

void NewsViewModel::requestRefresh()
{
    emit refreshRequested();
}

void NewsViewModel::updateCurrentFromEntry(const NewsEntryPtr& entry)
{
    QString newTitle;
    QString newContent;
    QString newLink;
    if (entry) {
        newTitle = entry->title;
        newContent = entry->content;
        newLink = entry->link;
    }
    const bool titleChanged = m_currentTitle != newTitle;
    const bool contentChanged = m_currentContent != newContent;
    const bool linkChanged = m_currentLink != newLink;
    if (!titleChanged && !contentChanged && !linkChanged) {
        return;
    }
    m_currentTitle = newTitle;
    m_currentContent = newContent;
    m_currentLink = newLink;
    if (contentChanged) {
        emit currentContentChanged();
    }
    if (titleChanged) {
        emit currentTitleChanged();
    }
    if (linkChanged) {
        emit currentLinkChanged();
    }
}
