// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
 *
 *  This file is part of ProjT Launcher and is licensed under
 *  the GNU General Public License version 3 or later.
 */

#include "ATLauncherViewModel.h"

#include <QSet>
#include <algorithm>
#include <optional>

#include "Application.h"
#include "InstanceList.h"
#include "modplatform/atlauncher/ATLPackIndex.h"
#include "modplatform/atlauncher/ATLPackInstallTask.h"
#include "tasks/Task.h"

namespace {
class DefaultAtlInteractionSupport : public ATLauncher::UserInteractionSupport {
   public:
    QString chooseVersion(Meta::VersionList::Ptr vlist, QString minecraftVersion) override
    {
        const auto versions = vlist->versions();
        for (const auto& version : versions) {
            const auto reqs = version->requiredSet();
            auto iter = std::find_if(reqs.begin(), reqs.end(), [](const Meta::Require& req) { return req.uid == "net.minecraft"; });
            if (iter != reqs.end() && !minecraftVersion.isEmpty() && iter->equalsVersion != minecraftVersion) {
                continue;
            }
            if (version->isRecommended()) {
                return version->descriptor();
            }
        }

        if (!versions.isEmpty()) {
            return versions.first()->descriptor();
        }
        return {};
    }

    std::optional<QList<QString>> chooseOptionalMods(const ATLauncher::PackVersion&, QList<ATLauncher::VersionMod> mods) override
    {
        QList<QString> selected;
        for (const auto& mod : mods) {
            if (mod.hidden)
                continue;
            if (mod.selected || mod.recommended || mod.optional) {
                selected.append(mod.name);
            }
        }
        return selected;
    }

    void displayMessage([[maybe_unused]] QString message) override {}
};
}  // namespace

ATLauncherViewModel::ATLauncherViewModel(QObject* parent)
    : QObject(parent), m_listModel(new Atl::ListModel(this)), m_filterModel(new Atl::FilterModel(this))
{
    m_filterModel->setSourceModel(m_listModel);
    m_filterModel->setSorting(Atl::FilterModel::ByPopularity);

    connect(m_listModel, &Atl::ListModel::requestCompleted, this, &ATLauncherViewModel::onPacksLoaded);
    connect(m_listModel, &Atl::ListModel::requestError, this, &ATLauncherViewModel::onLoadError);

    // Initial load
    loadPacks();
}

ATLauncherViewModel::~ATLauncherViewModel() = default;

QAbstractItemModel* ATLauncherViewModel::packsModel() const
{
    return m_filterModel;
}

bool ATLauncherViewModel::isLoading() const
{
    return m_isLoading;
}

QString ATLauncherViewModel::statusMessage() const
{
    return m_statusMessage;
}

QString ATLauncherViewModel::searchTerm() const
{
    return m_searchTerm;
}

void ATLauncherViewModel::setSearchTerm(const QString& term)
{
    if (m_searchTerm == term)
        return;

    m_searchTerm = term;
    m_filterModel->setSearchTerm(term);
    emit searchTermChanged();
}

QString ATLauncherViewModel::selectedMinecraftVersion() const
{
    return m_selectedMinecraftVersion;
}

void ATLauncherViewModel::setSelectedMinecraftVersion(const QString& version)
{
    if (m_selectedMinecraftVersion == version)
        return;

    m_selectedMinecraftVersion = version;
    emit selectedMinecraftVersionChanged();
}

QStringList ATLauncherViewModel::minecraftVersions() const
{
    return m_minecraftVersions;
}

int ATLauncherViewModel::selectedPackIndex() const
{
    return m_selectedPackIndex;
}

void ATLauncherViewModel::setSelectedPackIndex(int index)
{
    if (m_selectedPackIndex == index)
        return;

    m_selectedPackIndex = index;
    updateSelectedPackInfo();
    emit selectedPackIndexChanged();
    emit selectedPackChanged();
}

QVariantMap ATLauncherViewModel::selectedPack() const
{
    return m_selectedPack;
}

QStringList ATLauncherViewModel::selectedPackVersions() const
{
    return m_selectedPackVersions;
}

int ATLauncherViewModel::selectedVersionIndex() const
{
    return m_selectedVersionIndex;
}

void ATLauncherViewModel::setSelectedVersionIndex(int index)
{
    if (m_selectedVersionIndex == index)
        return;

    m_selectedVersionIndex = index;
    emit selectedVersionIndexChanged();
}

void ATLauncherViewModel::refresh()
{
    loadPacks();
}

void ATLauncherViewModel::search(const QString& term)
{
    setSearchTerm(term);
}

void ATLauncherViewModel::selectPack(int index)
{
    setSelectedPackIndex(index);
}

void ATLauncherViewModel::selectVersion(int index)
{
    setSelectedVersionIndex(index);
}

void ATLauncherViewModel::installSelected([[maybe_unused]] const QString& instanceName, [[maybe_unused]] const QString& groupName)
{
    if (m_selectedPackIndex < 0 || m_selectedVersionIndex < 0) {
        emit installFinished(false, tr("No pack or version selected"));
        return;
    }

    emit installStarted();

    auto sourceIndex = m_filterModel->mapToSource(m_filterModel->index(m_selectedPackIndex, 0));
    if (!sourceIndex.isValid()) {
        emit installFinished(false, tr("Invalid pack selection"));
        return;
    }

    auto data = sourceIndex.data(Qt::UserRole);
    if (!data.canConvert<ATLauncher::IndexedPack>()) {
        emit installFinished(false, tr("Invalid pack selection"));
        return;
    }

    auto pack = data.value<ATLauncher::IndexedPack>();
    if (m_selectedVersionIndex >= pack.versions.size()) {
        emit installFinished(false, tr("Selected version is not available"));
        return;
    }

    auto version = pack.versions.at(m_selectedVersionIndex).version;
    auto support = new DefaultAtlInteractionSupport();
    auto installTask = new ATLauncher::PackInstallTask(support, pack.name, version);
    installTask->setName(instanceName);
    installTask->setGroup(groupName);

    auto wrappedTask = APPLICATION->instances()->wrapInstanceTask(installTask);
    if (!wrappedTask) {
        installTask->deleteLater();
        emit installFinished(false, tr("Failed to start installation task"));
        return;
    }

    auto finish = [this, wrappedTask](const QString& error) {
        if (!error.isEmpty()) {
            emit installFinished(false, error);
        } else {
            emit installFinished(true, tr("Installation completed"));
        }
        wrappedTask->deleteLater();
    };

    connect(wrappedTask, &Task::succeeded, this, [finish]() mutable { finish(QString()); });
    connect(wrappedTask, &Task::failed, this, [finish](const QString& reason) mutable { finish(reason); });
    connect(wrappedTask, &Task::aborted, this, [finish]() mutable { finish(tr("Installation aborted")); });

    wrappedTask->start();
}

void ATLauncherViewModel::clearSelection()
{
    m_selectedPackIndex = -1;
    m_selectedVersionIndex = -1;
    m_selectedPack.clear();
    m_selectedPackVersions.clear();

    emit selectedPackIndexChanged();
    emit selectedPackChanged();
    emit selectedPackVersionsChanged();
    emit selectedVersionIndexChanged();
}

QVariantMap ATLauncherViewModel::getPackInfo(int index) const
{
    if (index < 0 || index >= m_filterModel->rowCount())
        return {};

    auto modelIndex = m_filterModel->index(index, 0);
    QVariantMap info;
    info["name"] = modelIndex.data(Qt::DisplayRole).toString();
    info["description"] = modelIndex.data(Qt::ToolTipRole).toString();
    info["icon"] = modelIndex.data(Qt::DecorationRole);
    return info;
}

void ATLauncherViewModel::loadPacks()
{
    m_isLoading = true;
    m_statusMessage = tr("Loading ATLauncher packs...");
    emit isLoadingChanged();
    emit statusMessageChanged();

    m_listModel->request();
}

void ATLauncherViewModel::updateMinecraftVersions()
{
    QSet<QString> versions;
    for (int i = 0; i < m_listModel->rowCount(QModelIndex()); ++i) {
        auto index = m_listModel->index(i, 0);
        auto data = index.data(Qt::UserRole);
        if (data.canConvert<ATLauncher::IndexedPack>()) {
            auto pack = data.value<ATLauncher::IndexedPack>();
            for (const auto& ver : pack.versions) {
                versions.insert(ver.minecraft);
            }
        }
    }

    m_minecraftVersions = versions.values();
    m_minecraftVersions.sort();
    std::reverse(m_minecraftVersions.begin(), m_minecraftVersions.end());

    emit minecraftVersionsChanged();
}

void ATLauncherViewModel::updateSelectedPackInfo()
{
    if (m_selectedPackIndex < 0 || m_selectedPackIndex >= m_filterModel->rowCount()) {
        m_selectedPack.clear();
        m_selectedPackVersions.clear();
        m_selectedVersionIndex = -1;
        return;
    }

    auto modelIndex = m_filterModel->index(m_selectedPackIndex, 0);
    auto sourceIndex = m_filterModel->mapToSource(modelIndex);
    auto data = sourceIndex.data(Qt::UserRole);

    if (data.canConvert<ATLauncher::IndexedPack>()) {
        auto pack = data.value<ATLauncher::IndexedPack>();
        m_selectedPack["name"] = pack.name;
        m_selectedPack["description"] = pack.description;
        m_selectedPack["safeName"] = pack.safeName;
        m_selectedPack["id"] = pack.id;

        m_selectedPackVersions.clear();
        for (const auto& ver : pack.versions) {
            m_selectedPackVersions.append(QString("%1 (%2)").arg(ver.version, ver.minecraft));
        }

        if (!m_selectedPackVersions.isEmpty() && m_selectedVersionIndex < 0) {
            m_selectedVersionIndex = 0;
        }
    }

    emit selectedPackVersionsChanged();
}

void ATLauncherViewModel::onPacksLoaded()
{
    m_isLoading = false;
    m_statusMessage = tr("Packs loaded successfully");
    updateMinecraftVersions();
    emit isLoadingChanged();
    emit statusMessageChanged();
}

void ATLauncherViewModel::onLoadError(const QString& error)
{
    m_isLoading = false;
    m_statusMessage = tr("Failed to load packs: %1").arg(error);
    emit isLoadingChanged();
    emit statusMessageChanged();
}

void ATLauncherViewModel::onSelectionChanged()
{
    updateSelectedPackInfo();
    emit selectedPackChanged();
}
