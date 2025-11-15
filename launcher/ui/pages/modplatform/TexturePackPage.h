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
 *
 * === Upstream License Block (Do Not Modify) ==============================
 *
 * // SPDX-FileCopyrightText: 2023 flowln <flowlnlnln@gmail.com>
 * //
 * // SPDX-License-Identifier: GPL-3.0-only
 *
 * ======================================================================== */

#pragma once

#include "ui/dialogs/ResourceDownloadDialog.h"
#include "ui/pages/modplatform/ResourcePackPage.h"
#include "ui/pages/modplatform/TexturePackModel.h"
#include "ui_ResourcePage.h"

namespace Ui {
class ResourcePage;
}

namespace ResourceDownload {

class TexturePackDownloadDialog;

class TexturePackResourcePage : public ResourcePackResourcePage {
    Q_OBJECT

   public:
    template <typename T>
    static T* create(TexturePackDownloadDialog* dialog, BaseInstance& instance)
    {
        auto page = new T(dialog, instance);
        auto model = static_cast<TexturePackResourceModel*>(page->getModel());

        connect(model, &ResourceModel::versionListUpdated, page, &ResourcePage::versionListUpdated);
        connect(model, &ResourceModel::projectInfoUpdated, page, &ResourcePage::updateUi);
        connect(model, &QAbstractListModel::modelReset, page, &ResourcePage::modelReset);

        return page;
    }

    //: The plural version of 'texture pack'
    inline QString resourcesString() const override { return tr("texture packs"); }
    //: The singular version of 'texture packs'
    inline QString resourceString() const override { return tr("texture pack"); }

   protected:
    TexturePackResourcePage(TexturePackDownloadDialog* dialog, BaseInstance& instance) : ResourcePackResourcePage(dialog, instance) {}
};

}  // namespace ResourceDownload
