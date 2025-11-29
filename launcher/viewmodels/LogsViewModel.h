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

#include <QObject>
#include <QString>
#include <QStringList>
#include <QFileSystemWatcher>
#include <QFont>

#include "BaseInstance.h"
#include "launch/LogModel.h"
#include "ui/pages/instance/LogPage.h"

class LogsViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString logText READ logText NOTIFY logTextChanged)
    Q_PROPERTY(QString category READ category NOTIFY categoryChanged)
    Q_PROPERTY(bool busy READ isBusy WRITE setBusy NOTIFY busyChanged)
    Q_PROPERTY(QString busyReason READ busyReason NOTIFY busyChanged)
    Q_PROPERTY(QString selectedLog READ selectedLog WRITE setSelectedLog NOTIFY selectedLogChanged)
    Q_PROPERTY(QStringList logList READ logList NOTIFY logListChanged)
    Q_PROPERTY(bool tailing READ isTailing WRITE setTailing NOTIFY tailingChanged)
    Q_PROPERTY(QAbstractItemModel* logModel READ logModel NOTIFY logModelChanged)
    Q_PROPERTY(QAbstractItemModel* proxyModel READ proxyModel NOTIFY logModelChanged)

   public:
    explicit LogsViewModel(QObject* parent = nullptr);

    QString logText() const;
    QString category() const;
    bool isBusy() const;
    QString busyReason() const;
    QString selectedLog() const { return m_selectedLog; }
    QStringList logList() const { return m_logList; }
    bool isTailing() const { return m_tailing; }
    QAbstractItemModel* logModel() const;
    QAbstractItemModel* proxyModel() const;

    void setLogText(const QString& text);
    void setCategory(const QString& category);
    void setBusy(bool busy, const QString& reason = {});
    void setBusyReason(const QString& reason);
    void setSelectedLog(const QString& logId);
    void setLogList(const QStringList& logs);
    void setTailing(bool tailing);

    Q_INVOKABLE void loadLogs(const QString& logId = {});
    Q_INVOKABLE void tailLogs(const QString& logId = {});
    Q_INVOKABLE void clearLogs(const QString& logId = {});
    void setWrapLines(bool wrap);
    void setColorLines(bool color);
    void setSuspended(bool suspended);
    void setFont(const QFont& font);
    void configure(const InstancePtr& instance, const QString& basePath, const QStringList& searchPaths,
                   const shared_qobject_ptr<LogModel>& launcherModel = shared_qobject_ptr<LogModel>());

   signals:
    void started(const QString& reason = {});
    void finished();
    void errorOccurred(const QString& message);
    void logTextChanged();
    void categoryChanged();
    void busyChanged();
    void logsUpdated();
    void logListChanged();
    void selectedLogChanged();
    void tailingChanged();
    void loadLogsRequested(const QString& logId);
    void tailLogsRequested(const QString& logId);
    void clearLogsRequested(const QString& logId);
    void logModelChanged();

   private:
    void bindModelToProxy();
    void refreshLogList();
    void loadFromFile(const QString& filePath);
    void clearModelOnly();
    SettingsObjectPtr currentSettings() const;

    QString m_logText;
    QString m_category;
    bool m_busy = false;
    QString m_busyReason;
    QString m_selectedLog;
    QStringList m_logList;
    bool m_tailing = false;
    InstancePtr m_instance;
    QString m_basePath;
    QStringList m_logSearchPaths;
    unique_qobject_ptr<QFileSystemWatcher> m_watcher;
    shared_qobject_ptr<LogModel> m_model;
    shared_qobject_ptr<LogModel> m_launcherModel;
    std::unique_ptr<LogFormatProxyModel> m_proxy;
};
