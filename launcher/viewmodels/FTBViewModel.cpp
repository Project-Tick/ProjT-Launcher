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

#include "FTBViewModel.h"

#include "Application.h"
#include "InstanceList.h"
#include "modplatform/legacy_ftb/PackFetchTask.h"
#include "modplatform/legacy_ftb/PackInstallTask.h"
#include "modplatform/legacy_ftb/PrivatePackManager.h"
#include "tasks/Task.h"

FTBViewModel::FTBViewModel(QObject* parent)
    : QObject(parent)
    , m_publicPacksModel(new LegacyFTB::ListModel(this))
    , m_publicFilterModel(new LegacyFTB::FilterModel(this))
    , m_privatePacksModel(new LegacyFTB::ListModel(this))
    , m_privateFilterModel(new LegacyFTB::FilterModel(this))
    , m_privatePackManager(std::make_unique<LegacyFTB::PrivatePackManager>())
{
    m_publicFilterModel->setSourceModel(m_publicPacksModel);
    m_privateFilterModel->setSourceModel(m_privatePacksModel);

    m_privatePackManager->load();

    m_fetchTask = std::make_shared<LegacyFTB::PackFetchTask>(APPLICATION->network());
    connect(m_fetchTask.get(), &LegacyFTB::PackFetchTask::finished, this, &FTBViewModel::onPackFetchFinished);
    connect(m_fetchTask.get(), &LegacyFTB::PackFetchTask::failed, this, &FTBViewModel::onPackFetchFailed);
    connect(m_fetchTask.get(), &LegacyFTB::PackFetchTask::aborted, this, &FTBViewModel::onPackFetchAborted);
    connect(m_fetchTask.get(), &LegacyFTB::PackFetchTask::privateFileDownloadFinished, this, &FTBViewModel::onPrivatePackFetched);
    connect(m_fetchTask.get(), &LegacyFTB::PackFetchTask::privateFileDownloadFailed, this, &FTBViewModel::onPrivatePackFetchFailed);

    // Initial load
    loadPacks();
}

FTBViewModel::~FTBViewModel() = default;

QAbstractItemModel* FTBViewModel::packsModel() const
{
    return m_publicFilterModel;
}

QAbstractItemModel* FTBViewModel::privatePacksModel() const
{
    return m_privateFilterModel;
}

bool FTBViewModel::isLoading() const
{
    return m_isLoading;
}

QString FTBViewModel::statusMessage() const
{
    return m_statusMessage;
}

QString FTBViewModel::searchTerm() const
{
    return m_searchTerm;
}

void FTBViewModel::setSearchTerm(const QString& term)
{
    if (m_searchTerm == term)
        return;

    m_searchTerm = term;
    m_publicFilterModel->setSearchTerm(term);
    m_privateFilterModel->setSearchTerm(term);
    emit searchTermChanged();
}

int FTBViewModel::selectedPackIndex() const
{
    return m_selectedPackIndex;
}

void FTBViewModel::setSelectedPackIndex(int index)
{
    if (m_selectedPackIndex == index)
        return;

    m_selectedPackIndex = index;
    updateSelectedPackInfo();
    emit selectedPackIndexChanged();
    emit selectedPackChanged();
}

QVariantMap FTBViewModel::selectedPack() const
{
    return m_selectedPack;
}

QStringList FTBViewModel::selectedPackVersions() const
{
    return m_selectedPackVersions;
}

int FTBViewModel::selectedVersionIndex() const
{
    return m_selectedVersionIndex;
}

void FTBViewModel::setSelectedVersionIndex(int index)
{
    if (m_selectedVersionIndex == index)
        return;

    m_selectedVersionIndex = index;
    emit selectedVersionIndexChanged();
}

bool FTBViewModel::showingPrivatePacks() const
{
    return m_showingPrivatePacks;
}

void FTBViewModel::setShowingPrivatePacks(bool showing)
{
    if (m_showingPrivatePacks == showing)
        return;

    m_showingPrivatePacks = showing;
    clearSelection();
    emit showingPrivatePacksChanged();
}

void FTBViewModel::refresh()
{
    loadPacks();
}

void FTBViewModel::search(const QString& term)
{
    setSearchTerm(term);
}

void FTBViewModel::selectPack(int index)
{
    setSelectedPackIndex(index);
}

void FTBViewModel::selectVersion(int index)
{
    setSelectedVersionIndex(index);
}

void FTBViewModel::installSelected([[maybe_unused]] const QString& instanceName, [[maybe_unused]] const QString& groupName)
{
    if (m_selectedPackIndex < 0) {
        emit installFinished(false, tr("No pack selected"));
        return;
    }

    auto pack = resolveSelectedPack();
    if (pack.name.isEmpty()) {
        emit installFinished(false, tr("Invalid pack selection"));
        return;
    }

    if (m_selectedVersionIndex < 0 || m_selectedVersionIndex >= m_selectedPackVersions.size()) {
        emit installFinished(false, tr("No version selected"));
        return;
    }

    emit installStarted();

    auto version = m_selectedPackVersions.at(m_selectedVersionIndex);
    auto installTask = new LegacyFTB::PackInstallTask(APPLICATION->network(), pack, version);
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

void FTBViewModel::clearSelection()
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

void FTBViewModel::addPrivatePack([[maybe_unused]] const QString& packCode)
{
    auto trimmedCode = packCode.trimmed();
    if (trimmedCode.isEmpty()) {
        emit privatePackAdded(false, tr("Pack code cannot be empty"));
        return;
    }

    m_privatePackManager->add(trimmedCode);
    m_privatePackManager->save();

    m_statusMessage = tr("Fetching private pack %1...").arg(trimmedCode);
    emit statusMessageChanged();
    m_fetchTask->fetchPrivate({ trimmedCode });
}

void FTBViewModel::removePrivatePack([[maybe_unused]] const QString& packCode)
{
    auto trimmedCode = packCode.trimmed();
    if (trimmedCode.isEmpty()) {
        return;
    }

    for (int row = 0; row < m_privatePacksModel->rowCount(QModelIndex()); ++row) {
        auto pack = m_privatePacksModel->at(row);
        if (pack.packCode == trimmedCode) {
            m_privatePacksModel->remove(row);
            break;
        }
    }

    m_privatePackManager->remove(trimmedCode);
    m_privatePackManager->save();

    if (m_showingPrivatePacks) {
        clearSelection();
    }
}

QVariantMap FTBViewModel::getPackInfo(int index) const
{
    auto model = currentModel();
    if (index < 0 || index >= model->rowCount())
        return {};

    auto modelIndex = model->index(index, 0);
    QVariantMap info;
    info["name"] = modelIndex.data(Qt::DisplayRole).toString();
    info["description"] = modelIndex.data(Qt::ToolTipRole).toString();
    info["icon"] = modelIndex.data(Qt::DecorationRole);
    return info;
}

void FTBViewModel::loadPacks()
{
    m_isLoading = true;
    m_statusMessage = tr("Loading FTB packs...");
    emit isLoadingChanged();
    emit statusMessageChanged();

    m_fetchTask->fetch();
    if (m_privatePackManager && !m_privatePackManager->empty()) {
        m_fetchTask->fetchPrivate(m_privatePackManager->getCurrentPackCodes().values());
    }
}

void FTBViewModel::updateSelectedPackInfo()
{
    auto model = currentModel();
    if (m_selectedPackIndex < 0 || m_selectedPackIndex >= model->rowCount()) {
        m_selectedPack.clear();
        m_selectedPackVersions.clear();
        m_selectedVersionIndex = -1;
        return;
    }

    auto modelIndex = model->index(m_selectedPackIndex, 0);
    auto filterModel = m_showingPrivatePacks ? m_privateFilterModel : m_publicFilterModel;
    auto sourceIndex = filterModel->mapToSource(modelIndex);
    
    auto listModel = m_showingPrivatePacks ? m_privatePacksModel : m_publicPacksModel;
    auto pack = listModel->at(sourceIndex.row());

    m_selectedPack["name"] = pack.name;
    m_selectedPack["description"] = pack.description;
    m_selectedPack["author"] = pack.author;
    m_selectedPack["mcVersion"] = pack.mcVersion;

    m_selectedPackVersions.clear();
    for (const auto& ver : pack.oldVersions) {
        m_selectedPackVersions.append(ver);
    }
    if (!m_selectedPackVersions.isEmpty() && m_selectedVersionIndex < 0) {
        m_selectedVersionIndex = 0;
    }

    emit selectedPackVersionsChanged();
}

void FTBViewModel::onPublicPacksLoaded(const LegacyFTB::ModpackList& packs)
{
    m_publicPacksModel->fill(packs);
    m_isLoading = false;
    m_statusMessage = tr("Packs loaded successfully");
    emit isLoadingChanged();
    emit statusMessageChanged();
}

void FTBViewModel::onPrivatePacksLoaded(const LegacyFTB::ModpackList& packs)
{
    m_privatePacksModel->fill(packs);
}

void FTBViewModel::onPackFetchFailed(const QString& error)
{
    m_isLoading = false;
    m_statusMessage = tr("Failed to load packs: %1").arg(error);
    emit isLoadingChanged();
    emit statusMessageChanged();
}

void FTBViewModel::onPackFetchFinished(const LegacyFTB::ModpackList& publicPacks, const LegacyFTB::ModpackList& thirdPartyPacks)
{
    m_publicPacksModel->fill(publicPacks + thirdPartyPacks);
    m_isLoading = false;
    m_statusMessage = tr("Packs loaded successfully");
    emit isLoadingChanged();
    emit statusMessageChanged();
}

void FTBViewModel::onPrivatePackFetched(const LegacyFTB::Modpack& pack)
{
    m_privatePacksModel->addPack(pack);
    m_statusMessage = tr("Added private pack %1").arg(pack.name);
    emit statusMessageChanged();
    emit privatePackAdded(true, m_statusMessage);
}

void FTBViewModel::onPrivatePackFetchFailed(const QString& error, const QString& packCode)
{
    m_statusMessage = tr("Failed to load pack %1: %2").arg(packCode, error);
    emit statusMessageChanged();
    emit privatePackAdded(false, m_statusMessage);
    m_privatePackManager->remove(packCode);
    m_privatePackManager->save();
}

void FTBViewModel::onPackFetchAborted()
{
    m_isLoading = false;
    m_statusMessage = tr("Pack fetch aborted");
    emit isLoadingChanged();
    emit statusMessageChanged();
}

QAbstractItemModel* FTBViewModel::currentModel() const
{
    return m_showingPrivatePacks ? m_privateFilterModel : m_publicFilterModel;
}

LegacyFTB::Modpack FTBViewModel::resolveSelectedPack() const
{
    auto model = currentModel();
    if (m_selectedPackIndex < 0 || m_selectedPackIndex >= model->rowCount()) {
        return {};
    }

    auto modelIndex = model->index(m_selectedPackIndex, 0);
    auto filterModel = m_showingPrivatePacks ? m_privateFilterModel : m_publicFilterModel;
    auto sourceIndex = filterModel->mapToSource(modelIndex);
    auto listModel = m_showingPrivatePacks ? m_privatePacksModel : m_publicPacksModel;

    return listModel->at(sourceIndex.row());
}
