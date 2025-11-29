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

#include "LogsViewModel.h"

#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QTextDocument>

#include <FileSystem.h>
#include <GZip.h>
#include "Application.h"
#include "QObjectPtr.h"
#include "logs/LogParser.h"

LogsViewModel::LogsViewModel(QObject* parent)
    : QObject(parent)
{
    m_watcher.reset(new QFileSystemWatcher());
    m_proxy = std::make_unique<LogFormatProxyModel>();
}

QString LogsViewModel::logText() const
{
    return m_logText;
}

QString LogsViewModel::category() const
{
    return m_category;
}

bool LogsViewModel::isBusy() const
{
    return m_busy;
}

QString LogsViewModel::busyReason() const
{
    return m_busyReason;
}

QAbstractItemModel* LogsViewModel::logModel() const
{
    return m_model.get();
}

QAbstractItemModel* LogsViewModel::proxyModel() const
{
    return m_proxy.get();
}

void LogsViewModel::setLogText(const QString& text)
{
    if (m_logText == text) {
        return;
    }
    m_logText = text;
    emit logTextChanged();
    emit logsUpdated();
}

void LogsViewModel::setCategory(const QString& category)
{
    if (m_category == category) {
        return;
    }
    m_category = category;
    emit categoryChanged();
}

void LogsViewModel::setBusy(bool busy, const QString& reason)
{
    if (m_busy == busy && m_busyReason == reason) {
        return;
    }
    m_busy = busy;
    m_busyReason = reason;
    emit busyChanged();
}

void LogsViewModel::setBusyReason(const QString& reason)
{
    if (m_busyReason == reason) {
        return;
    }
    m_busyReason = reason;
    emit busyChanged();
}

void LogsViewModel::setSelectedLog(const QString& logId)
{
    if (m_selectedLog == logId) {
        return;
    }
    m_selectedLog = logId;
    emit selectedLogChanged();
}

void LogsViewModel::setLogList(const QStringList& logs)
{
    if (m_logList == logs) {
        return;
    }
    m_logList = logs;
    emit logListChanged();
}

void LogsViewModel::setTailing(bool tailing)
{
    if (m_tailing == tailing) {
        return;
    }
    m_tailing = tailing;
    emit tailingChanged();
}

void LogsViewModel::loadLogs(const QString& logId)
{
    const QString target = logId.isEmpty() ? m_selectedLog : logId;
    setBusy(true, QObject::tr("Loading logs"));
    emit started(m_busyReason);
    loadFromFile(target);
    refreshLogList();
    setBusy(false);
    emit finished();
}

void LogsViewModel::tailLogs(const QString& logId)
{
    const QString target = logId.isEmpty() ? m_selectedLog : logId;
    setBusy(true, QObject::tr("Tailing logs"));
    emit started(m_busyReason);
    loadFromFile(target);
    setTailing(true);
    setBusy(false);
    emit finished();
}

void LogsViewModel::clearLogs(const QString& logId)
{
    const QString target = logId.isEmpty() ? m_selectedLog : logId;
    setBusy(true, QObject::tr("Clearing logs"));
    emit started(m_busyReason);
    Q_UNUSED(target);
    clearModelOnly();
    refreshLogList();
    setBusy(false);
    emit finished();
}

void LogsViewModel::setWrapLines(bool wrap)
{
    if (m_model) {
        m_model->setLineWrap(wrap);
    }
}

void LogsViewModel::setColorLines(bool color)
{
    if (m_model) {
        m_model->setColorLines(color);
    }
}

void LogsViewModel::setSuspended(bool suspended)
{
    if (m_model) {
        m_model->suspend(suspended);
    }
}

void LogsViewModel::setFont(const QFont& font)
{
    if (m_proxy) {
        m_proxy->setFont(font);
    }
}

void LogsViewModel::configure(const InstancePtr& instance, const QString& basePath, const QStringList& searchPaths,
                              const shared_qobject_ptr<LogModel>& launcherModel)
{
    m_instance = instance;
    m_basePath = basePath;
    m_logSearchPaths = searchPaths;
    m_launcherModel = launcherModel;

    if (!m_watcher) {
        m_watcher.reset(new QFileSystemWatcher());
    }
    m_watcher->removePaths(m_watcher->directories());
    if (!m_logSearchPaths.isEmpty()) {
        m_watcher->addPaths(m_logSearchPaths);
    }

    if (m_instance) {
        m_model.reset(new LogModel(this));
        m_model->setMaxLines(getConsoleMaxLines(currentSettings()));
        m_model->setStopOnOverflow(shouldStopOnConsoleOverflow(currentSettings()));
        m_model->setOverflowMessage(tr("Cannot display this log since the log length surpassed %1 lines.")
                                        .arg(m_model->getMaxLines()));
    } else if (m_launcherModel) {
        m_model = m_launcherModel;
    } else {
        m_model.reset(new LogModel(this));
    }

    if (!m_proxy) {
        m_proxy = std::make_unique<LogFormatProxyModel>();
    }
    bindModelToProxy();

    // initialise font/wrap/color from settings
    if (auto settings = currentSettings()) {
        const QString fontFamily = settings->get("ConsoleFont").toString();
        bool ok = false;
        int fontSize = settings->get("ConsoleFontSize").toInt(&ok);
        if (!ok) {
            fontSize = 11;
        }
        setFont(QFont(fontFamily, fontSize));
    }
    refreshLogList();
}

void LogsViewModel::bindModelToProxy()
{
    if (m_proxy) {
        m_proxy->setSourceModel(m_model.get());
        emit logModelChanged();
    }
}

void LogsViewModel::refreshLogList()
{
    QStringList result;
    QDir baseDir(m_basePath);
    for (const QString& searchPath : m_logSearchPaths) {
        QDir searchDir(searchPath);
        QStringList filters{ "*.log", "*.log.gz" };
        if (searchPath != m_basePath) {
            filters.append("*.txt");
        }
        QStringList entries = searchDir.entryList(filters, QDir::Files | QDir::Readable, QDir::SortFlag::Time);
        for (const QString& name : entries) {
            result.append(baseDir.relativeFilePath(searchDir.filePath(name)));
        }
    }
    setLogList(result);
}

void LogsViewModel::loadFromFile(const QString& filePath)
{
    QString target = filePath;
    if (target.isEmpty()) {
        // launcher log view if no specific file
        if (!m_instance && m_model) {
            setSelectedLog(QString());
            setCategory(tr("Launcher"));
            setLogText(QString());
            emit logsUpdated();
            return;
        }
        emit errorOccurred(tr("No log selected."));
        return;
    }
    setSelectedLog(target);
    setCategory(target);

    const QString absolutePath = FS::PathCombine(m_basePath, target);
    QFile file(absolutePath);
    if (!file.open(QFile::ReadOnly)) {
        emit errorOccurred(tr("Unable to open %1 for reading: %2").arg(target, file.errorString()));
        return;
    }
    if (!m_model) {
        m_model.reset(new LogModel(this));
    }
    m_model->clear();
    QStringList plainLines;
    MessageLevel::Enum last = MessageLevel::Unknown;

    auto handleLine = [&](QString line) {
        if (line.isEmpty()) {
            return false;
        }
        if (line.back() == '\n') {
            line.chop(1);
        }
        MessageLevel::Enum level = MessageLevel::Unknown;
        QString lineTemp = line;
        if (!m_instance) {
            level = MessageLevel::fromLauncherLine(lineTemp);
        } else {
            auto innerLevel = MessageLevel::fromLine(lineTemp);
            if (innerLevel != MessageLevel::Unknown) {
                level = innerLevel;
            }
            if (level == MessageLevel::StdErr || level == MessageLevel::StdOut || level == MessageLevel::Unknown) {
                level = LogParser::guessLevel(line, last);
            }
        }
        last = level;
        m_model->append(level, line);
        plainLines.push_back(line);
        return m_model->isOverFlow();
    };

    if (file.fileName().endsWith(".gz")) {
        QString line;
        auto error = GZip::readGzFileByBlocks(&file, [&line, &handleLine](const QByteArray& d) {
            auto block = d;
            int newlineIndex = block.indexOf('\n');
            while (newlineIndex != -1) {
                line += QString::fromUtf8(block).left(newlineIndex);
                block.remove(0, newlineIndex + 1);
                if (handleLine(line)) {
                    line.clear();
                    return false;
                }
                line.clear();
                newlineIndex = block.indexOf('\n');
            }
            line += QString::fromUtf8(block);
            return true;
        });
        if (!error.isEmpty()) {
            emit errorOccurred(tr("The file (%1) encountered an error when reading: %2.").arg(file.fileName(), error));
            return;
        } else if (!line.isEmpty()) {
            handleLine(line);
        }
    } else {
        while (!file.atEnd() && !handleLine(QString::fromUtf8(file.readLine()))) {
        }
    }

    bindModelToProxy();
    setLogText(plainLines.join('\n'));
    emit logsUpdated();
}

void LogsViewModel::clearModelOnly()
{
    if (m_model) {
        m_model->clear();
    }
    setLogText(QString());
    emit logsUpdated();
}

SettingsObjectPtr LogsViewModel::currentSettings() const
{
    if (m_instance) {
        return m_instance->settings();
    }
    return APPLICATION->settings();
}
