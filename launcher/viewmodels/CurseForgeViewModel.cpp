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
 *
 */

#include "CurseForgeViewModel.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "Application.h"
#include "InstanceImportTask.h"
#include "InstanceList.h"
#include "modplatform/flame/FlameAPI.h"
#include "net/ApiDownload.h"

static FlameAPI s_api;

// ============== CurseForgePackListModel ==============

CurseForgePackListModel::CurseForgePackListModel(QObject* parent) : QAbstractListModel(parent) {}

int CurseForgePackListModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : m_packs.size();
}

QVariant CurseForgePackListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_packs.size())
        return {};

    auto pack = m_packs.at(index.row());

    switch (role) {
        case NameRole:
        case Qt::DisplayRole:
            return pack->name;
        case DescriptionRole:
            return pack->description;
        case AuthorRole:
            if (!pack->authors.isEmpty())
                return pack->authors.first().name;
            return QString();
        case IconUrlRole:
            return pack->logoUrl;
        case DownloadsRole:
            return QString();  // Not available in IndexedPack
        case PackDataRole:
            return QVariant::fromValue(pack);
        default:
            return {};
    }
}

QHash<int, QByteArray> CurseForgePackListModel::roleNames() const
{
    return { { NameRole, "name" },       { DescriptionRole, "description" }, { AuthorRole, "author" },
             { IconUrlRole, "iconUrl" }, { DownloadsRole, "downloads" },     { PackDataRole, "packData" } };
}

bool CurseForgePackListModel::canFetchMore(const QModelIndex& parent) const
{
    return !parent.isValid() && m_searchState == CanPossiblyFetchMore;
}

void CurseForgePackListModel::fetchMore(const QModelIndex& parent)
{
    if (parent.isValid() || m_searchState != CanPossiblyFetchMore)
        return;

    performPaginatedSearch();
}

void CurseForgePackListModel::searchWithTerm(const QString& term, int sort)
{
    if (m_jobPtr && m_jobPtr->isRunning()) {
        m_jobPtr->abort();
    }

    m_currentSearchTerm = term;
    m_currentSort = sort;
    m_nextSearchOffset = 0;
    m_searchState = None;

    beginResetModel();
    m_packs.clear();
    endResetModel();

    performPaginatedSearch();
}

ModPlatform::IndexedPack::Ptr CurseForgePackListModel::packAt(int index) const
{
    if (index < 0 || index >= m_packs.size())
        return nullptr;
    return m_packs.at(index);
}

void CurseForgePackListModel::clear()
{
    beginResetModel();
    m_packs.clear();
    m_searchState = None;
    endResetModel();
}

void CurseForgePackListModel::performPaginatedSearch()
{
    ResourceAPI::SortingMethod sort{};
    sort.index = m_currentSort + 1;

    ResourceAPI::Callback<QList<ModPlatform::IndexedPack::Ptr>> callbacks;
    callbacks.on_succeed = [this](QList<ModPlatform::IndexedPack::Ptr>& packs) { searchRequestFinished(packs); };
    callbacks.on_fail = [this](QString reason, int) { searchRequestFailed(reason); };

    m_jobPtr = s_api.searchProjects(
        { ModPlatform::ResourceType::Modpack, m_nextSearchOffset, m_currentSearchTerm, sort, {}, {}, {}, {}, false }, std::move(callbacks));

    if (m_jobPtr) {
        m_jobPtr->start();
    }
}

void CurseForgePackListModel::searchRequestFinished(QList<ModPlatform::IndexedPack::Ptr>& packs)
{
    if (packs.size() < 25) {
        m_searchState = Finished;
    } else {
        m_nextSearchOffset += 25;
        m_searchState = CanPossiblyFetchMore;
    }

    if (packs.isEmpty()) {
        emit searchFinished();
        return;
    }

    beginInsertRows(QModelIndex(), m_packs.size(), m_packs.size() + packs.size() - 1);
    m_packs.append(packs);
    endInsertRows();

    emit searchFinished();
}

void CurseForgePackListModel::searchRequestFailed(const QString& reason)
{
    m_searchState = Finished;
    emit searchFailed(reason);
}

// ============== CurseForgeViewModel ==============

CurseForgeViewModel::CurseForgeViewModel(QObject* parent) : QObject(parent), m_model(new CurseForgePackListModel(this))
{
    m_searchTimer.setTimerType(Qt::TimerType::CoarseTimer);
    m_searchTimer.setSingleShot(true);
    m_searchTimer.setInterval(350);

    connect(&m_searchTimer, &QTimer::timeout, this, &CurseForgeViewModel::triggerSearch);
    connect(m_model, &CurseForgePackListModel::searchFinished, this, &CurseForgeViewModel::onSearchFinished);
    connect(m_model, &CurseForgePackListModel::searchFailed, this, [this](const QString& reason) {
        setLoading(false);
        setStatusMessage(tr("Search failed: %1").arg(reason));
    });

    // Load categories on init
    loadCategories();
}

CurseForgeViewModel::~CurseForgeViewModel() = default;

QAbstractItemModel* CurseForgeViewModel::packsModel() const
{
    return m_model;
}

void CurseForgeViewModel::setSearchTerm(const QString& term)
{
    if (m_searchTerm != term) {
        m_searchTerm = term;
        emit searchTermChanged();
        m_searchTimer.start();
    }
}

void CurseForgeViewModel::setSortIndex(int index)
{
    if (m_sortIndex != index) {
        m_sortIndex = index;
        emit sortIndexChanged();
        triggerSearch();
    }
}

QStringList CurseForgeViewModel::sortOptions() const
{
    return { tr("Featured"), tr("Popularity"), tr("Last Updated"), tr("Name"), tr("Author"), tr("Total Downloads") };
}

void CurseForgeViewModel::setSelectedCategoryIndex(int index)
{
    if (m_selectedCategoryIndex != index) {
        m_selectedCategoryIndex = index;
        emit selectedCategoryIndexChanged();
        triggerSearch();
    }
}

void CurseForgeViewModel::setSelectedVersionIndex(int index)
{
    if (m_selectedVersionIndex != index) {
        m_selectedVersionIndex = index;
        emit selectedVersionIndexChanged();
    }
}

void CurseForgeViewModel::refresh()
{
    triggerSearch();
}

void CurseForgeViewModel::search(const QString& term)
{
    setSearchTerm(term);
}

void CurseForgeViewModel::selectPack(int index)
{
    if (m_selectedPackIndex == index)
        return;

    m_selectedPackIndex = index;
    emit selectedPackIndexChanged();

    m_currentPack = m_model->packAt(index);
    updateSelectedPackInfo();

    if (m_currentPack && !m_currentPack->versionsLoaded) {
        loadVersionsForPack();
    }
}

void CurseForgeViewModel::selectVersion(int index)
{
    setSelectedVersionIndex(index);
}

void CurseForgeViewModel::installSelected(const QString& instanceName, const QString& groupName)
{
    Q_UNUSED(groupName);

    if (!m_currentPack || m_selectedVersionIndex < 0)
        return;

    if (m_selectedVersionIndex >= m_currentPack->versions.size())
        return;

    auto& version = m_currentPack->versions.at(m_selectedVersionIndex);

    QMap<QString, QString> extraInfo;
    extraInfo.insert("pack_id", m_currentPack->addonId.toString());
    extraInfo.insert("pack_version_id", version.fileId.toString());

    auto task = new InstanceImportTask(version.downloadUrl, nullptr, std::move(extraInfo));
    task->setName(instanceName.isEmpty() ? m_currentPack->name : instanceName);

    QString packName = m_currentPack->name;
    emit installStarted(packName);

    APPLICATION->instances()->wrapInstanceTask(task);
}

void CurseForgeViewModel::clearSelection()
{
    m_selectedPackIndex = -1;
    m_selectedVersionIndex = -1;
    m_currentPack = nullptr;
    m_selectedPack.clear();
    m_selectedPackVersions.clear();

    emit selectedPackIndexChanged();
    emit selectedPackChanged();
    emit selectedPackVersionsChanged();
    emit selectedVersionIndexChanged();
}

void CurseForgeViewModel::onSearchFinished()
{
    setLoading(false);
    setStatusMessage(QString());
}

void CurseForgeViewModel::onVersionsLoaded()
{
    if (!m_currentPack)
        return;

    m_selectedPackVersions.clear();
    for (const auto& version : m_currentPack->versions) {
        m_selectedPackVersions << version.getVersionDisplayString();
    }

    emit selectedPackVersionsChanged();

    if (!m_selectedPackVersions.isEmpty()) {
        setSelectedVersionIndex(0);
    }
}

void CurseForgeViewModel::triggerSearch()
{
    clearSelection();
    setLoading(true);
    setStatusMessage(tr("Searching..."));
    m_model->searchWithTerm(m_searchTerm, m_sortIndex);
}

void CurseForgeViewModel::loadCategories()
{
    auto response = std::make_shared<QByteArray>();
    auto job = FlameAPI::getCategories(response, ModPlatform::ResourceType::Modpack);

    connect(job.get(), &Task::succeeded, this, [this, response]() {
        auto cats = FlameAPI::loadModCategories(response);
        m_categories.clear();
        m_categories << tr("All Categories");
        for (const auto& cat : cats) {
            m_categories << cat.name;
        }
        emit categoriesChanged();
    });

    job->start();
}

void CurseForgeViewModel::setLoading(bool loading)
{
    if (m_isLoading != loading) {
        m_isLoading = loading;
        emit isLoadingChanged();
    }
}

void CurseForgeViewModel::setStatusMessage(const QString& message)
{
    if (m_statusMessage != message) {
        m_statusMessage = message;
        emit statusMessageChanged();
    }
}

void CurseForgeViewModel::updateSelectedPackInfo()
{
    m_selectedPack.clear();
    m_selectedPackVersions.clear();
    m_selectedVersionIndex = -1;

    if (!m_currentPack) {
        emit selectedPackChanged();
        emit selectedPackVersionsChanged();
        emit selectedVersionIndexChanged();
        return;
    }

    m_selectedPack["name"] = m_currentPack->name;
    m_selectedPack["description"] = m_currentPack->description;
    m_selectedPack["iconUrl"] = m_currentPack->logoUrl;
    m_selectedPack["websiteUrl"] = m_currentPack->websiteUrl;
    m_selectedPack["downloadCount"] = QString();

    if (!m_currentPack->authors.isEmpty()) {
        QStringList authorNames;
        for (const auto& author : m_currentPack->authors) {
            authorNames << author.name;
        }
        m_selectedPack["authors"] = authorNames.join(", ");
    } else {
        m_selectedPack["authors"] = QString();
    }

    emit selectedPackChanged();

    if (m_currentPack->versionsLoaded) {
        for (const auto& version : m_currentPack->versions) {
            m_selectedPackVersions << version.getVersionDisplayString();
        }
        emit selectedPackVersionsChanged();

        if (!m_selectedPackVersions.isEmpty()) {
            setSelectedVersionIndex(0);
        }
    }
}

void CurseForgeViewModel::loadVersionsForPack()
{
    if (!m_currentPack)
        return;

    ResourceAPI::Callback<QVector<ModPlatform::IndexedVersion>> callbacks;
    auto addonId = m_currentPack->addonId;

    callbacks.on_succeed = [this, addonId](QVector<ModPlatform::IndexedVersion>& versions) {
        if (!m_currentPack || m_currentPack->addonId != addonId)
            return;

        m_currentPack->versions = versions;
        m_currentPack->versionsLoaded = true;
        onVersionsLoaded();
    };

    callbacks.on_fail = [this](QString reason, int) { setStatusMessage(tr("Failed to load versions: %1").arg(reason)); };

    m_versionsJob = s_api.getProjectVersions({ m_currentPack, {}, {}, ModPlatform::ResourceType::Modpack }, std::move(callbacks));
    if (m_versionsJob) {
        m_versionsJob->start();
    }
}
