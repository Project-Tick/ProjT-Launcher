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
#include <QtNetwork/qtcpsocket.h>
#include <QDnsLookup>
#include <QHostInfo>
#include <QObject>
#include <QString>

// resolve the IP and port of a Minecraft server
class McResolver : public QObject {
    Q_OBJECT

    QString m_constrDomain;
    int m_constrPort;

   public:
    explicit McResolver(QObject* parent, QString domain, int port);
    void ping();

   private:
    void pingWithDomainSRV(QString domain, int port);
    void pingWithDomainA(QString domain, int port);
    void emitFail(QString error);
    void emitSucceed(QString ip, int port);

   signals:
    void succeeded(QString ip, int port);
    void failed(QString error);
    void finished();
};
