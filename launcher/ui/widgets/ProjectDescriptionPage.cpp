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
#include "ProjectDescriptionPage.h"

#include "VariableSizedImageObject.h"

#include <QDebug>

ProjectDescriptionPage::ProjectDescriptionPage(QWidget* parent) : QTextBrowser(parent), m_image_text_object(new VariableSizedImageObject)
{
    m_image_text_object->setParent(this);
    document()->documentLayout()->registerHandler(QTextFormat::ImageObject, m_image_text_object.get());
}

void ProjectDescriptionPage::setMetaEntry(QString entry)
{
    if (m_image_text_object)
        m_image_text_object->setMetaEntry(entry);
}

void ProjectDescriptionPage::flush()
{
    if (m_image_text_object)
        m_image_text_object->flush();
}
