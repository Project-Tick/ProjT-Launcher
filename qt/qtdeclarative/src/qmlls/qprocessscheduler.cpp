// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qprocessscheduler_p.h"
#include <QtQmlDom/private/qqmldom_utils_p.h>

QT_BEGIN_NAMESPACE

namespace QmlLsp {

Q_STATIC_LOGGING_CATEGORY(schedulerLog, "qt.languageserver.qprocessscheduler")

using namespace Qt::StringLiterals;

/*!
  \internal
  \class QProcessScheduler

  \brief Runs multiple processes sequentially via a QProcess, and signals once they are done.

  QProcessScheduler runs multiple programs sequentially through a QProcess, and emits the \c done
  signal once all programs finished. Use schedule(programs, id) to add new programs to be run. The
  id is used to signal their completion. Duplicate programs that already are in the queue are not
  re-added to the queue.
 */
QProcessScheduler::QProcessScheduler()
{
    QObject::connect(&m_process, &QProcess::finished, this, &QProcessScheduler::processNext);
    QObject::connect(&m_process, &QProcess::errorOccurred, this,
                     &QProcessScheduler::onErrorOccurred);
}
QProcessScheduler::~QProcessScheduler()
{
    QObject::disconnect(&m_process, nullptr);
    m_process.kill();
    m_process.waitForFinished();
}

QT_WARNING_PUSH
QT_WARNING_DISABLE_GCC("-Wmaybe-uninitialized") // use of std::variant
void QProcessScheduler::schedule(const QList<Command> &list, const Id &id)
{
    m_queue.enqueue(StartMarker{ id });
    const int queueSize = m_queue.size();
    for (const auto &x : list) {
        const QueueElement queueElement{ x };
        if (!m_queue.contains(queueElement))
            m_queue.enqueue(queueElement);
    }

    // remove startmarker if no queue element was added, instead just emit that this task was done
    if (queueSize == m_queue.size()) {
        m_queue.removeLast(); // remove start marker
    }

    m_queue.enqueue(EndMarker{ id });
    if (!std::exchange(m_isRunning, true))
        QMetaObject::invokeMethod(this, &QProcessScheduler::processNext, Qt::QueuedConnection);
}
QT_WARNING_POP

static bool isStartMarkerOf(const QProcessScheduler::QueueElement &e,
                            const QProcessScheduler::Id &id)
{
    const auto startMarker = std::get_if<QProcessScheduler::StartMarker>(&e);
    if (!startMarker)
        return false;
    return startMarker->id == id;
}

static bool isEndMarkerOf(const QProcessScheduler::QueueElement &e, const QProcessScheduler::Id &id)
{
    const auto endMarker = std::get_if<QProcessScheduler::EndMarker>(&e);
    if (!endMarker)
        return false;
    return endMarker->id == id;
}

void QProcessScheduler::cancel(const Id &id)
{
    auto removeQueueElementsForStartMarkerIt = [this, &id](auto begin) {
        auto end = std::find_if(begin, m_queue.cend(),
                                std::bind(isEndMarkerOf, std::placeholders::_1, id));

        // also include the marker during erase
        if (end != m_queue.cend())
            std::advance(end, 1);
        m_queue.erase(begin, end);
    };

    if (m_current != id) {
        const auto begin = std::find_if(m_queue.cbegin(), m_queue.cend(),
                                        std::bind(isStartMarkerOf, std::placeholders::_1, id));
        removeQueueElementsForStartMarkerIt(begin);
        emit cancelled(id);
        return;
    }

    {
        QObject::disconnect(&m_process, &QProcess::finished, this, &QProcessScheduler::processNext);
        QObject::disconnect(&m_process, &QProcess::errorOccurred, this,
                            &QProcessScheduler::onErrorOccurred);
        // note: kill and waitForFinished triggers the finished signal, which we don't want to trigger
        m_process.kill();
        m_process.waitForFinished();
        removeQueueElementsForStartMarkerIt(m_queue.cbegin());
        QObject::connect(&m_process, &QProcess::finished, this, &QProcessScheduler::processNext);
        QObject::connect(&m_process, &QProcess::errorOccurred, this,
                         &QProcessScheduler::onErrorOccurred);
    }

    emit cancelled(id);
    if (!std::exchange(m_isRunning, true))
        QMetaObject::invokeMethod(this, &QProcessScheduler::processNext, Qt::QueuedConnection);
}

void QProcessScheduler::processNext()
{
    m_isRunning = m_queue.size() > 0;
    if (!m_isRunning)
        return;

    std::visit(qOverloadedVisitor{
                       [this](const StartMarker &start) {
                           emit started(start.id);
                           m_current = start.id;
                           processNext();
                       },
                       [this](const EndMarker &end) {
                           m_current.reset();
                           emit done(end.id);
                           processNext();
                       },
                       [this](const Command &command) {
                           m_process.setProgram(command.program);
                           m_process.setArguments(command.arguments);
                           m_process.setProcessEnvironment(command.customEnvironment);
                           m_process.start();
                       },
               },
               m_queue.dequeue());
}

void QProcessScheduler::onErrorOccurred(QProcess::ProcessError error)
{
    qCDebug(schedulerLog) << "Process" << m_process.program() << m_process.arguments().join(" "_L1)
                          << "had an error:" << error;
    processNext();
}

} // namespace QmlLsp

QT_END_NAMESPACE
