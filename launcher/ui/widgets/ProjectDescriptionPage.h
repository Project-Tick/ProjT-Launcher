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

#include <QTextBrowser>

#include "QObjectPtr.h"

QT_BEGIN_NAMESPACE
class VariableSizedImageObject;
QT_END_NAMESPACE

/** This subclasses QTextBrowser to provide additional capabilities
 *  to it, like allowing for images to be shown.
 */
class ProjectDescriptionPage final : public QTextBrowser {
    Q_OBJECT

   public:
    ProjectDescriptionPage(QWidget* parent = nullptr);

    void setMetaEntry(QString entry);

   public slots:
    /** Flushes the current processing happening in the page.
     *
     *  Should be called when changing the page's content entirely, to
     *  prevent old tasks from changing the new content.
     */
    void flush();

   private:
    shared_qobject_ptr<VariableSizedImageObject> m_image_text_object;
};
