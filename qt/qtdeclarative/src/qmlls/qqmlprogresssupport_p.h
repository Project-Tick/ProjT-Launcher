// Copyright (C) 2026 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default

#ifndef QQMLPROGRESS_P_H
#define QQMLPROGRESS_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qlanguageserver_p.h"
#include "qqmlcodemodelmanager_p.h"

QT_BEGIN_NAMESPACE

class QQmlProgressSupport final : public QLanguageServerModule
{
    Q_OBJECT
public:
    QQmlProgressSupport(QmlLsp::QQmlCodeModelManager *manager);

    QString name() const final;
    void registerHandlers(QLanguageServer *server, QLanguageServerProtocol *protocol) final;
    void setupCapabilities(const QLspSpecification::InitializeParams &clientInfo,
                           QLspSpecification::InitializeResult &) final;

private:
    struct UriWithToken
    {
        QByteArray uri;
        int token;
    };

    int createUniqueToken(const QByteArray &uri);

    QmlLsp::QQmlCodeModelManager *m_codeModelManager;
    QList<UriWithToken> m_tokens;
    QLanguageServerProtocol *m_protocol;
    int m_idForBackgroundBuilds = 0;

private slots:
    void onBackgroundBuildStarted(const QByteArray &uri);
    void onBackgroundBuildDone(const QByteArray &uri);
    void onBackgroundBuildCancelRequested(
            const QLspSpecification::Notifications::WorkDoneProgressCancelParamsType &p);
    void clientInitialized(QLanguageServer *server);
};

QT_END_NAMESPACE

#endif // QQMLPROGRESS_P_H
