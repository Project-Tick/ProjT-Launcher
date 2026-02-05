// Copyright (C) 2026 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default

#include <QtCore/qstring.h>

#include "qqmlprogresssupport_p.h"

using namespace Qt::StringLiterals;

QT_BEGIN_NAMESPACE

QQmlProgressSupport::QQmlProgressSupport(QmlLsp::QQmlCodeModelManager *manager)
    : m_codeModelManager(manager)
{
}

QString QQmlProgressSupport::name() const
{
    return "QQmlProgress"_L1;
}

void QQmlProgressSupport::registerHandlers(QLanguageServer *server,
                                           QLanguageServerProtocol *protocol)
{
    m_protocol = protocol;
    QObject::connect(server, &QLanguageServer::clientInitialized, this,
                     &QQmlProgressSupport::clientInitialized);
}

void QQmlProgressSupport::clientInitialized(QLanguageServer *server)
{
    if (auto window = server->clientInfo().capabilities.window;
        !window || !window->value("workDoneProgress"_L1).toBool(false)) {
        return;
    }

    QObject::connect(m_codeModelManager, &QmlLsp::QQmlCodeModelManager::backgroundBuildStarted,
                     this, &QQmlProgressSupport::onBackgroundBuildStarted);
    QObject::connect(m_codeModelManager, &QmlLsp::QQmlCodeModelManager::backgroundBuildFinished,
                     this, &QQmlProgressSupport::onBackgroundBuildDone);

    QObject::connect(server->notifySignals(),
                     &QLspNotifySignals::receivedWorkDoneProgressCancelNotification, this,
                     &QQmlProgressSupport::onBackgroundBuildCancelRequested);
}

int QQmlProgressSupport::createUniqueToken(const QByteArray &uri)
{
    const int token = m_idForBackgroundBuilds++;
    m_tokens.append({ uri, token });
    return token;
}

void QQmlProgressSupport::onBackgroundBuildStarted(const QByteArray &uri)
{
    QLspSpecification::Requests::WorkDoneProgressCreateParamsType p;
    const int token = createUniqueToken(uri);
    p.token = token;
    m_protocol->requestWorkDoneProgressCreate(p, [this, uri, token]() {
        QLspSpecification::ProgressParams beginParams{ token };
        QLspSpecification::WorkDoneProgressBegin workDoneProgressBegin{};
        workDoneProgressBegin.title = "Qmlls running background build";
        workDoneProgressBegin.cancellable = true;
        workDoneProgressBegin.message =
                "Building \"" + QUrl::fromEncoded(uri).toLocalFile().toUtf8() + "\"";
        workDoneProgressBegin.cancellable = true;
        beginParams.value = workDoneProgressBegin;
        m_protocol->notifyProgress(beginParams);
    });
}

void QQmlProgressSupport::onBackgroundBuildDone(const QByteArray &uri)
{
    const auto it =
            std::find_if(m_tokens.begin(), m_tokens.end(), [uri](const UriWithToken &uriWithToken) {
                return uriWithToken.uri == uri;
            });

    if (it == m_tokens.end())
        return;

    QLspSpecification::WorkDoneProgressEnd workDoneProgressEnd;
    workDoneProgressEnd.message = "Build terminated";
    const QLspSpecification::ProgressParams endParams{ it->token, workDoneProgressEnd };
    m_protocol->notifyProgress(endParams);
    m_tokens.erase(it);
}

void QQmlProgressSupport::onBackgroundBuildCancelRequested(
        const QLspSpecification::Notifications::WorkDoneProgressCancelParamsType &p)
{
    const auto token = std::get_if<int>(&p.token);
    if (!token)
        return;

    const auto it = std::find_if(
            m_tokens.begin(), m_tokens.end(),
            [token](const UriWithToken &uriWithToken) { return uriWithToken.token == *token; });

    if (it == m_tokens.end())
        return;

    m_codeModelManager->cancelBackgroundBuild(it->uri);
    m_tokens.erase(it);
}

void QQmlProgressSupport::setupCapabilities(const QLspSpecification::InitializeParams &,
                                            QLspSpecification::InitializeResult &)
{
}

QT_END_NAMESPACE
