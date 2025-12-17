// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#pragma once
#include <QProcess>
#include <QTimer>

#include "JavaVersion.h"
#include "QObjectPtr.h"
#include "tasks/Task.h"

class JavaChecker : public Task {
    Q_OBJECT
   public:
    using QProcessPtr = shared_qobject_ptr<QProcess>;
    using Ptr = shared_qobject_ptr<JavaChecker>;

    struct Result {
        QString path;
        int id;
        QString mojangPlatform;
        QString realPlatform;
        JavaVersion javaVersion;
        QString javaVendor;
        QString outLog;
        QString errorLog;
        bool is_64bit = false;
        enum class Validity { Errored, ReturnedInvalidData, Valid } validity = Validity::Errored;
    };

    explicit JavaChecker(QString path, QString args, int minMem = 0, int maxMem = 0, int permGen = 0, int id = 0);

   signals:
    void checkFinished(const Result& result);

   protected:
    virtual void executeTask() override;

   private:
    QProcessPtr process;
    QTimer killTimer;
    QString m_stdout;
    QString m_stderr;

    QString m_path;
    QString m_args;
    int m_minMem = 0;
    int m_maxMem = 0;
    int m_permGen = 64;
    int m_id = 0;

   private slots:
    void timeout();
    void finished(int exitcode, QProcess::ExitStatus);
    void error(QProcess::ProcessError);
    void stdoutReady();
    void stderrReady();
};
