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

#pragma once

#include "InstanceTask.h"
#include "net/NetJob.h"

#include <quazip/quazip.h>

#include <QFutureWatcher>
#include <QStringList>
#include <QUrl>

#include <optional>

namespace Technic {

class SingleZipPackInstallTask : public InstanceTask {
    Q_OBJECT

   public:
    SingleZipPackInstallTask(const QUrl& sourceUrl, const QString& minecraftVersion);

    bool canAbort() const override { return true; }
    bool abort() override;

   protected:
    void executeTask() override;

   private slots:
    void downloadSucceeded();
    void downloadFailed(QString reason);
    void downloadProgressChanged(qint64 current, qint64 total);
    void extractFinished();
    void extractAborted();

   private:
    bool m_abortable = false;

    QUrl m_sourceUrl;
    QString m_minecraftVersion;
    QString m_archivePath;
    NetJob::Ptr m_filesNetJob;
    std::unique_ptr<QuaZip> m_packZip;
    QFuture<std::optional<QStringList>> m_extractFuture;
    QFutureWatcher<std::optional<QStringList>> m_extractFutureWatcher;
};

}  // namespace Technic
