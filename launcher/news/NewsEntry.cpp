// SPDX-License-Identifier: GPL-3.0-or-later AND Apache-2.0
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
 *
 * === Upstream License Block (Do Not Modify) ==============================
 *
 * Copyright 2013-2021 MultiMC Contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ======================================================================== */

#include "NewsEntry.h"

#include <QDomNodeList>
#include <QVariant>

NewsEntry::NewsEntry(QObject* parent) : QObject(parent)
{
    this->title = tr("Untitled");
    this->content = tr("No content.");
    this->link = "";
}

NewsEntry::NewsEntry(const QString& title, const QString& content, const QString& link, QObject* parent) : QObject(parent)
{
    this->title = title;
    this->content = content;
    this->link = link;
}

/*!
 * Gets the text content of the given child element as a QVariant.
 */
inline QString childValue(const QDomElement& element, const QString& childName, QString defaultVal = "")
{
    QDomNodeList nodes = element.elementsByTagName(childName);
    if (nodes.count() > 0) {
        QDomElement elem = nodes.at(0).toElement();
        return elem.text();
    } else {
        return defaultVal;
    }
}

bool NewsEntry::fromXmlElement(const QDomElement& element, NewsEntry* entry, [[maybe_unused]] QString* errorMsg)
{
    QString title = childValue(element, "title", tr("Untitled"));
    QString content = childValue(element, "content", tr("No content."));
    QString link = childValue(element, "id");

    entry->title = title;
    entry->content = content;
    entry->link = link;
    return true;
}
