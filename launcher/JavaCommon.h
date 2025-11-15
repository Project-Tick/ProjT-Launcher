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
#include <java/JavaChecker.h>

class QWidget;

/**
 * Common UI bits for the java pages to use.
 */
namespace JavaCommon {
bool checkJVMArgs(QString args, QWidget* parent);

// Show a dialog saying that the Java binary was usable
void javaWasOk(QWidget* parent, const JavaChecker::Result& result);
// Show a dialog saying that the Java binary was not usable because of bad options
void javaArgsWereBad(QWidget* parent, const JavaChecker::Result& result);
// Show a dialog saying that the Java binary was not usable
void javaBinaryWasBad(QWidget* parent, const JavaChecker::Result& result);
// Show a dialog if we couldn't find Java Checker
void javaCheckNotFound(QWidget* parent);

class TestCheck : public QObject {
    Q_OBJECT
   public:
    TestCheck(QWidget* parent, QString path, QString args, int minMem, int maxMem, int permGen)
        : m_parent(parent), m_path(path), m_args(args), m_minMem(minMem), m_maxMem(maxMem), m_permGen(permGen)
    {}
    virtual ~TestCheck() = default;

    void run();

   signals:
    void finished();

   private slots:
    void checkFinished(const JavaChecker::Result& result);
    void checkFinishedWithArgs(const JavaChecker::Result& result);

   private:
    JavaChecker::Ptr checker;
    QWidget* m_parent = nullptr;
    QString m_path;
    QString m_args;
    int m_minMem = 0;
    int m_maxMem = 0;
    int m_permGen = 64;
};
}  // namespace JavaCommon
