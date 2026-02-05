// Copyright (C) 2026 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "tst_qmlls_progress_p.h"

#include <QtLanguageServer/private/qlanguageserverspectypes_p.h>
#include <QtQmlLS/private/qqmllanguageserver_p.h>
#include <QtTest/qsignalspy.h>
#include <QtTest/qtest.h>

using namespace QmlLsp;
using namespace QLspSpecification;

void tst_qmlls_progress::backgroundBuild_data()
{
    QTest::addColumn<std::function<void(QLanguageServerProtocol *, bool *)>>("registerCheck");

    QTest::addRow("createNotification") << std::function{ [](QLanguageServerProtocol *client,
                                                             bool *ok) {
        client->registerWorkDoneProgressCreateRequestHandler(
                [ok](const QByteArray &,
                     const Requests::WorkDoneProgressCreateParamsType &paramsToCheck,
                     LSPResponse<Responses::WorkDoneProgressCreateResultType> &&response) {
                    QCOMPARE(std::get<int>(paramsToCheck.token), 0);
                    *ok = true;
                    response.sendResponse();
                });
    } };

    QTest::addRow("startNotification") << std::function{ [](QLanguageServerProtocol *client,
                                                            bool *ok) {
        client->registerWorkDoneProgressCreateRequestHandler(
                [](const QByteArray &, const Requests::WorkDoneProgressCreateParamsType &,
                   LSPResponse<Responses::WorkDoneProgressCreateResultType> &&response) {
                    response.sendResponse();
                });
        client->registerProgressNotificationHandler([ok](const QByteArray &,
                                                         const ProgressParams &paramsToCheck) {
            QCOMPARE(std::get<int>(paramsToCheck.token), 0);
            if (std::holds_alternative<WorkDoneProgressBegin>(paramsToCheck.value)) {
                QVERIFY(!*ok);
                WorkDoneProgressBegin paramValueToCheck =
                        std::get<WorkDoneProgressBegin>(paramsToCheck.value);
                QCOMPARE(paramValueToCheck.message, "Building \"\"");
                QCOMPARE(paramValueToCheck.cancellable, true);
                *ok = true;
            }
        });
    } };

    QTest::addRow("endNotification") << std::function{ [](QLanguageServerProtocol *client,
                                                          bool *ok) {
        client->registerWorkDoneProgressCreateRequestHandler(
                [](const QByteArray &, const Requests::WorkDoneProgressCreateParamsType &,
                   LSPResponse<Responses::WorkDoneProgressCreateResultType> &&response) {
                    response.sendResponse();
                });
        client->registerProgressNotificationHandler(
                [ok](const QByteArray &, const ProgressParams &paramsToCheck) {
                    QCOMPARE(std::get<int>(paramsToCheck.token), 0);

                    if (std::holds_alternative<WorkDoneProgressEnd>(paramsToCheck.value)) {
                        QVERIFY(!*ok);
                        QCOMPARE(std::get<WorkDoneProgressEnd>(paramsToCheck.value).message,
                                 "Build terminated");
                        *ok = true;
                    }
                });
    } };
}

void tst_qmlls_progress::backgroundBuild()
{
    QFETCH(std::function<void(QLanguageServerProtocol *, bool *)>, registerCheck);

    std::unique_ptr<QLanguageServerProtocol> client;
    std::unique_ptr<QQmlLanguageServer> server;

    client = std::make_unique<QLanguageServerProtocol>(
                 [&server](const QByteArray &data) { server->server()->receiveData(data, true); });
    server = std::make_unique<QQmlLanguageServer>(
                 [&client](const QByteArray &data) { client->receiveData(data); });

    bool initializedOk = false;
    InitializeParams initializeParams;
    initializeParams.capabilities.window.emplace().insert("workDoneProgress", true);
    client->requestInitialize(initializeParams,
                              [&initializedOk](const InitializeResult &) { initializedOk = true; });
    QTRY_VERIFY_WITH_TIMEOUT(initializedOk, 3000);
    client->notifyInitialized({});

    bool ok = false;
    registerCheck(client.get(), &ok);

    // simulate build trigger:
    server->codeModelManager()->backgroundBuildStarted("");
    server->codeModelManager()->backgroundBuildFinished("");

    QTRY_VERIFY_WITH_TIMEOUT(ok, 3000);
}

void tst_qmlls_progress::cancelBackgroundBuild()
{
    std::unique_ptr<QLanguageServerProtocol> client;
    std::unique_ptr<QQmlLanguageServer> server;

    client = std::make_unique<QLanguageServerProtocol>(
            [&server](const QByteArray &data) { server->server()->receiveData(data, true); });
    server = std::make_unique<QQmlLanguageServer>(
            [&client](const QByteArray &data) { client->receiveData(data); });

    bool initializedOk = false;
    InitializeParams initializeParams;
    initializeParams.capabilities.window.emplace().insert("workDoneProgress", true);
    client->requestInitialize(initializeParams,
                              [&initializedOk](const InitializeResult &) { initializedOk = true; });
    QTRY_VERIFY_WITH_TIMEOUT(initializedOk, 3000);
    client->notifyInitialized({});

    client->registerWorkDoneProgressCreateRequestHandler(
            [](const QByteArray &, const Requests::WorkDoneProgressCreateParamsType &,
               LSPResponse<Responses::WorkDoneProgressCreateResultType> &&response) {
                response.sendResponse();
            });
    client->registerProgressNotificationHandler(
            [](const QByteArray &, const ProgressParams &paramsToCheck) {
                QCOMPARE(std::get<int>(paramsToCheck.token), 0);
            });

    QSignalSpy spy(server->codeModelManager(), &QQmlCodeModelManager::backgroundBuildCancelled);

    // simulate build trigger
    server->codeModelManager()->backgroundBuildStarted("");

    QCOMPARE(spy.count(), 0);

    WorkDoneProgressCancelParams p;
    p.token = 0;
    client->notifyWorkDoneProgressCancel(p);

    QTRY_COMPARE_WITH_TIMEOUT(spy.count(), 1, 3000);
}

QTEST_MAIN(tst_qmlls_progress)
