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

#include <QByteArray>
#include <QObject>
#include <QTemporaryDir>
#include <memory>
#include "PackHelpers.h"
#include "net/NetJob.h"

namespace LegacyFTB {

class PackFetchTask : public QObject {
    Q_OBJECT

   public:
    PackFetchTask(shared_qobject_ptr<QNetworkAccessManager> network) : QObject(nullptr), m_network(network) {};
    virtual ~PackFetchTask() = default;

    void fetch();
    void fetchPrivate(const QStringList& toFetch);

   private:
    shared_qobject_ptr<QNetworkAccessManager> m_network;
    NetJob::Ptr jobPtr;

    std::shared_ptr<QByteArray> publicModpacksXmlFileData = std::make_shared<QByteArray>();
    std::shared_ptr<QByteArray> thirdPartyModpacksXmlFileData = std::make_shared<QByteArray>();

    bool parseAndAddPacks(QByteArray& data, PackType packType, ModpackList& list);
    ModpackList publicPacks;
    ModpackList thirdPartyPacks;

   protected slots:
    void fileDownloadFinished();
    void fileDownloadFailed(QString reason);
    void fileDownloadAborted();

   signals:
    void finished(ModpackList publicPacks, ModpackList thirdPartyPacks);
    void failed(QString reason);
    void aborted();

    void privateFileDownloadFinished(const Modpack& modpack);
    void privateFileDownloadFailed(QString reason, QString packCode);
};

}  // namespace LegacyFTB
