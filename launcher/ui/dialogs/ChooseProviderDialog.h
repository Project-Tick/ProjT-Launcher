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

#include <QButtonGroup>
#include <QDialog>

namespace Ui {
class ChooseProviderDialog;
}

namespace ModPlatform {
enum class ResourceProvider;
}

class Mod;
class NetJob;

class ChooseProviderDialog : public QDialog {
    Q_OBJECT

    struct Response {
        bool skip_all = false;
        bool confirm_all = false;

        bool try_others = false;

        ModPlatform::ResourceProvider chosen;
    };

   public:
    explicit ChooseProviderDialog(QWidget* parent, bool single_choice = false, bool allow_skipping = true);
    ~ChooseProviderDialog();

    auto getResponse() const -> Response { return m_response; }

    void setDescription(QString desc);

   private slots:
    void skipOne();
    void skipAll();
    void confirmOne();
    void confirmAll();

   private:
    void addProviders();
    void disableInput();

    auto getSelectedProvider() const -> ModPlatform::ResourceProvider;

   private:
    Ui::ChooseProviderDialog* ui;

    QButtonGroup m_providers;

    Response m_response;
};
