// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2025 Project Tick

#include "ModrinthViewModel.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "Application.h"
#include "InstanceImportTask.h"
#include "InstanceList.h"
#include "modplatform/modrinth/ModrinthAPI.h"
#include "net/ApiDownload.h"

static ModrinthAPI s_api;

// ============== ModrinthPackListModel ==============

ModrinthPackListModel::ModrinthPackListModel(QObject* parent) : QAbstractListModel(parent) {}

int ModrinthPackListModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : m_packs.size();
}

QVariant ModrinthPackListModel::data(const QModelIndex& index, int role) const
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
    case FollowsRole:
        return QString();  // Not available in IndexedPack
    case PackDataRole:
        return QVariant::fromValue(pack);
    default:
        return {};
    }
}

QHash<int, QByteArray> ModrinthPackListModel::roleNames() const
{
    return { { NameRole, "name" },           { DescriptionRole, "description" }, { AuthorRole, "author" },    { IconUrlRole, "iconUrl" },
             { DownloadsRole, "downloads" }, { FollowsRole, "follows" },         { PackDataRole, "packData" } };
}

bool ModrinthPackListModel::canFetchMore(const QModelIndex& parent) const
{
    return !parent.isValid() && m_searchState == CanPossiblyFetchMore;
}

void ModrinthPackListModel::fetchMore(const QModelIndex& parent)
{
    if (parent.isValid() || m_searchState != CanPossiblyFetchMore)
        return;

    performPaginatedSearch();
}

void ModrinthPackListModel::searchWithTerm(const QString& term, int sort)
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

ModPlatform::IndexedPack::Ptr ModrinthPackListModel::packAt(int index) const
{
    if (index < 0 || index >= m_packs.size())
        return nullptr;
    return m_packs.at(index);
}

void ModrinthPackListModel::clear()
{
    beginResetModel();
    m_packs.clear();
    m_searchState = None;
    endResetModel();
}

static auto sortFromIndex(int index) -> QString
{
    switch (index) {
    default:
    case 0:
        return "relevance";
    case 1:
        return "downloads";
    case 2:
        return "follows";
    case 3:
        return "newest";
    case 4:
        return "updated";
    }
}

void ModrinthPackListModel::performPaginatedSearch()
{
    ResourceAPI::SortingMethod sort{};
    sort.name = sortFromIndex(m_currentSort);

    ResourceAPI::Callback<QList<ModPlatform::IndexedPack::Ptr>> callbacks;
    callbacks.on_succeed = [this](QList<ModPlatform::IndexedPack::Ptr>& packs) { searchRequestFinished(packs); };
    callbacks.on_fail = [this](QString reason, int) { searchRequestFailed(reason); };

    m_jobPtr = s_api.searchProjects(
        { ModPlatform::ResourceType::Modpack, m_nextSearchOffset, m_currentSearchTerm, sort, {}, {}, {}, {}, false }, std::move(callbacks));

    if (m_jobPtr) {
        m_jobPtr->start();
    }
}

void ModrinthPackListModel::searchRequestFinished(QList<ModPlatform::IndexedPack::Ptr>& packs)
{
    if (packs.size() < 20) {
        m_searchState = Finished;
    } else {
        m_nextSearchOffset += 20;
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

void ModrinthPackListModel::searchRequestFailed(const QString& reason)
{
    m_searchState = Finished;
    emit searchFailed(reason);
}

// ============== ModrinthViewModel ==============

ModrinthViewModel::ModrinthViewModel(QObject* parent) : QObject(parent), m_model(new ModrinthPackListModel(this))
{
    m_searchTimer.setTimerType(Qt::TimerType::CoarseTimer);
    m_searchTimer.setSingleShot(true);
    m_searchTimer.setInterval(350);

    connect(&m_searchTimer, &QTimer::timeout, this, &ModrinthViewModel::triggerSearch);
    connect(m_model, &ModrinthPackListModel::searchFinished, this, &ModrinthViewModel::onSearchFinished);
    connect(m_model, &ModrinthPackListModel::searchFailed, this, [this](const QString& reason) {
        setLoading(false);
        setStatusMessage(tr("Search failed: %1").arg(reason));
    });

    // Load categories on init
    loadCategories();
}

ModrinthViewModel::~ModrinthViewModel() = default;

QAbstractItemModel* ModrinthViewModel::packsModel() const
{
    return m_model;
}

void ModrinthViewModel::setSearchTerm(const QString& term)
{
    if (m_searchTerm != term) {
        m_searchTerm = term;
        emit searchTermChanged();
        m_searchTimer.start();
    }
}

void ModrinthViewModel::setSortIndex(int index)
{
    if (m_sortIndex != index) {
        m_sortIndex = index;
        emit sortIndexChanged();
        triggerSearch();
    }
}

QStringList ModrinthViewModel::sortOptions() const
{
    return { tr("Relevance"), tr("Downloads"), tr("Follows"), tr("Newest"), tr("Updated") };
}

void ModrinthViewModel::setSelectedCategoryIndex(int index)
{
    if (m_selectedCategoryIndex != index) {
        m_selectedCategoryIndex = index;
        emit selectedCategoryIndexChanged();
        triggerSearch();
    }
}

QStringList ModrinthViewModel::loaders() const
{
    return { tr("Any Loader"), "Forge", "Fabric", "Quilt", "NeoForge" };
}

void ModrinthViewModel::setSelectedLoaderIndex(int index)
{
    if (m_selectedLoaderIndex != index) {
        m_selectedLoaderIndex = index;
        emit selectedLoaderIndexChanged();
        triggerSearch();
    }
}

void ModrinthViewModel::setSelectedVersionIndex(int index)
{
    if (m_selectedVersionIndex != index) {
        m_selectedVersionIndex = index;
        emit selectedVersionIndexChanged();
    }
}

void ModrinthViewModel::refresh()
{
    triggerSearch();
}

void ModrinthViewModel::search(const QString& term)
{
    setSearchTerm(term);
}

void ModrinthViewModel::selectPack(int index)
{
    if (m_selectedPackIndex == index)
        return;

    m_selectedPackIndex = index;
    emit selectedPackIndexChanged();

    m_currentPack = m_model->packAt(index);
    updateSelectedPackInfo();

    if (m_currentPack) {
        if (!m_currentPack->extraDataLoaded) {
            loadExtraInfoForPack();
        }
        if (!m_currentPack->versionsLoaded) {
            loadVersionsForPack();
        }
    }
}

void ModrinthViewModel::selectVersion(int index)
{
    setSelectedVersionIndex(index);
}

void ModrinthViewModel::installSelected(const QString& instanceName, const QString& groupName)
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

void ModrinthViewModel::clearSelection()
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

void ModrinthViewModel::onSearchFinished()
{
    setLoading(false);
    setStatusMessage(QString());
}

void ModrinthViewModel::onVersionsLoaded()
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

void ModrinthViewModel::onExtraInfoLoaded()
{
    updateSelectedPackInfo();
}

void ModrinthViewModel::triggerSearch()
{
    clearSelection();
    setLoading(true);
    setStatusMessage(tr("Searching..."));
    m_model->searchWithTerm(m_searchTerm, m_sortIndex);
}

void ModrinthViewModel::loadCategories()
{
    auto response = std::make_shared<QByteArray>();
    auto job = ModrinthAPI::getModCategories(response);

    connect(job.get(), &Task::succeeded, this, [this, response]() {
        auto cats = ModrinthAPI::loadCategories(response, "modpack");
        m_categories.clear();
        m_categories << tr("All Categories");
        for (const auto& cat : cats) {
            m_categories << cat.name;
        }
        emit categoriesChanged();
    });

    job->start();
}

void ModrinthViewModel::setLoading(bool loading)
{
    if (m_isLoading != loading) {
        m_isLoading = loading;
        emit isLoadingChanged();
    }
}

void ModrinthViewModel::setStatusMessage(const QString& message)
{
    if (m_statusMessage != message) {
        m_statusMessage = message;
        emit statusMessageChanged();
    }
}

void ModrinthViewModel::updateSelectedPackInfo()
{
    m_selectedPack.clear();

    if (!m_currentPack) {
        emit selectedPackChanged();
        return;
    }

    m_selectedPack["name"] = m_currentPack->name;
    m_selectedPack["description"] = m_currentPack->description;
    m_selectedPack["iconUrl"] = m_currentPack->logoUrl;
    m_selectedPack["websiteUrl"] = m_currentPack->websiteUrl;
    m_selectedPack["downloadCount"] = QString();
    m_selectedPack["followsCount"] = QString();

    if (!m_currentPack->authors.isEmpty()) {
        QStringList authorNames;
        for (const auto& author : m_currentPack->authors) {
            authorNames << author.name;
        }
        m_selectedPack["authors"] = authorNames.join(", ");
    } else {
        m_selectedPack["authors"] = QString();
    }

    // Extra data
    if (m_currentPack->extraDataLoaded) {
        m_selectedPack["body"] = m_currentPack->extraData.body;
        m_selectedPack["issuesUrl"] = m_currentPack->extraData.issuesUrl;
        m_selectedPack["sourceUrl"] = m_currentPack->extraData.sourceUrl;
        m_selectedPack["wikiUrl"] = m_currentPack->extraData.wikiUrl;
        m_selectedPack["discordUrl"] = m_currentPack->extraData.discordUrl;
        m_selectedPack["status"] = m_currentPack->extraData.status;
    }

    emit selectedPackChanged();
}

void ModrinthViewModel::loadVersionsForPack()
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

void ModrinthViewModel::loadExtraInfoForPack()
{
    if (!m_currentPack)
        return;

    ResourceAPI::Callback<ModPlatform::IndexedPack::Ptr> callbacks;
    auto addonId = m_currentPack->addonId;

    callbacks.on_succeed = [this, addonId](ModPlatform::IndexedPack::Ptr& pack) {
        if (!m_currentPack || m_currentPack->addonId != addonId)
            return;

        // Copy extra data
        m_currentPack->extraData = pack->extraData;
        m_currentPack->extraDataLoaded = true;
        onExtraInfoLoaded();
    };

    callbacks.on_fail = [this](QString reason, int) { qWarning() << "Failed to load extra info:" << reason; };

    m_infoJob = s_api.getProjectInfo({ m_currentPack }, std::move(callbacks));
    if (m_infoJob) {
        m_infoJob->start();
    }
}
