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
    enum WorkDoneProgressStatus { InCreation, Created, Finished };
    struct UriWithToken
    {
        QByteArray uri;
        int token = -1;
        WorkDoneProgressStatus status = InCreation;
    };

    class Tokens
    {
    public:
        int createUniqueToken(const QByteArray &uri);
        void removeToken(const QByteArray &uri);
        UriWithToken *find(const QByteArray &uri);
        std::optional<QQmlProgressSupport::UriWithToken> takeToken(int token);

    private:
        QHash<QByteArray, UriWithToken> m_tokens;
        QHash<int, QByteArray> m_uriByToken;
        int m_idForBackgroundBuilds = 0;
    };

    Tokens m_tokens;
    QmlLsp::QQmlCodeModelManager *m_codeModelManager = nullptr;
    QLanguageServerProtocol *m_protocol = nullptr;

private slots:
    void onBackgroundBuildStarted(const QByteArray &uri);
    void onBackgroundBuildDone(const QByteArray &uri);
    void onBackgroundBuildCancelRequested(
            const QLspSpecification::Notifications::WorkDoneProgressCancelParamsType &p);
    void clientInitialized(QLanguageServer *server);
};

QT_END_NAMESPACE

#endif // QQMLPROGRESS_P_H
