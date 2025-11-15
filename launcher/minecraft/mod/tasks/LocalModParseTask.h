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

#include <QDebug>
#include <QObject>

#include "minecraft/mod/Mod.h"
#include "minecraft/mod/ModDetails.h"

#include "tasks/Task.h"

namespace ModUtils {

ModDetails ReadFabricModInfo(QByteArray contents);
ModDetails ReadQuiltModInfo(QByteArray contents);
ModDetails ReadForgeInfo(QByteArray contents);
ModDetails ReadLiteModInfo(QByteArray contents);

enum class ProcessingLevel { Full, BasicInfoOnly };

bool process(Mod& mod, ProcessingLevel level = ProcessingLevel::Full);

bool processZIP(Mod& mod, ProcessingLevel level = ProcessingLevel::Full);
bool processFolder(Mod& mod, ProcessingLevel level = ProcessingLevel::Full);
bool processLitemod(Mod& mod, ProcessingLevel level = ProcessingLevel::Full);

/** Checks whether a file is valid as a mod or not. */
bool validate(QFileInfo file);

bool processIconPNG(const Mod& mod, QByteArray&& raw_data, QPixmap* pixmap);
bool loadIconFile(const Mod& mod, QPixmap* pixmap);
}  // namespace ModUtils

class LocalModParseTask : public Task {
    Q_OBJECT
   public:
    struct Result {
        ModDetails details;
    };
    using ResultPtr = std::shared_ptr<Result>;
    ResultPtr result() const { return m_result; }

    bool canAbort() const override { return true; }
    bool abort() override;

    LocalModParseTask(int token, ResourceType type, const QFileInfo& modFile);
    void executeTask() override;

    int token() const { return m_token; }

   private:
    int m_token;
    ResourceType m_type;
    QFileInfo m_modFile;
    ResultPtr m_result;

    std::atomic<bool> m_aborted = false;
};
