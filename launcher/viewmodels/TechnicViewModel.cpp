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

#include "TechnicViewModel.h"

#include "Application.h"
#include "BuildConfig.h"
#include "InstanceList.h"
#include "Json.h"
#include "modplatform/technic/SingleZipPackInstallTask.h"
#include "modplatform/technic/SolderPackInstallTask.h"
#include "modplatform/technic/SolderPackManifest.h"
#include "net/ApiDownload.h"
#include "tasks/Task.h"
#include "ui/pages/modplatform/technic/TechnicData.h"

TechnicViewModel::TechnicViewModel(QObject* parent) : QObject(parent), m_listModel(new Technic::ListModel(this))
{
    // No initial search - wait for user input
    m_statusMessage = tr("Enter a search term to find Technic packs");
}

TechnicViewModel::~TechnicViewModel() = default;

QAbstractItemModel* TechnicViewModel::packsModel() const
{
    return m_listModel;
}

bool TechnicViewModel::isLoading() const
{
    return m_isLoading;
}

QString TechnicViewModel::statusMessage() const
{
    return m_statusMessage;
}

QString TechnicViewModel::searchTerm() const
{
    return m_searchTerm;
}

void TechnicViewModel::setSearchTerm(const QString& term)
{
    if (m_searchTerm == term)
        return;

    m_searchTerm = term;
    emit searchTermChanged();

    // Trigger search when term changes
    if (!term.isEmpty()) {
        search(term);
    }
}

int TechnicViewModel::selectedPackIndex() const
{
    return m_selectedPackIndex;
}

void TechnicViewModel::setSelectedPackIndex(int index)
{
    if (m_selectedPackIndex == index)
        return;

    m_selectedPackIndex = index;
    updateSelectedPackInfo();
    emit selectedPackIndexChanged();
    emit selectedPackChanged();
}

QVariantMap TechnicViewModel::selectedPack() const
{
    return m_selectedPack;
}

QStringList TechnicViewModel::selectedPackVersions() const
{
    return m_selectedPackVersions;
}

int TechnicViewModel::selectedVersionIndex() const
{
    return m_selectedVersionIndex;
}

void TechnicViewModel::setSelectedVersionIndex(int index)
{
    if (m_selectedVersionIndex == index)
        return;

    m_selectedVersionIndex = index;
    emit selectedVersionIndexChanged();
}

bool TechnicViewModel::isLoadingMetadata() const
{
    return m_isLoadingMetadata;
}

bool TechnicViewModel::hasActiveSearch() const
{
    return m_listModel->hasActiveSearchJob();
}

void TechnicViewModel::refresh()
{
    if (!m_searchTerm.isEmpty()) {
        search(m_searchTerm);
    }
}

void TechnicViewModel::search(const QString& term)
{
    m_isLoading = true;
    m_statusMessage = tr("Searching for '%1'...").arg(term);
    emit isLoadingChanged();
    emit statusMessageChanged();
    emit hasActiveSearchChanged();

    m_listModel->searchWithTerm(term);

    // Check if search job is active, otherwise update status
    if (!m_listModel->hasActiveSearchJob()) {
        m_isLoading = false;
        m_statusMessage = tr("Search complete");
        emit isLoadingChanged();
        emit statusMessageChanged();
        emit hasActiveSearchChanged();
    }
}

void TechnicViewModel::selectPack(int index)
{
    setSelectedPackIndex(index);
}

void TechnicViewModel::selectVersion(int index)
{
    setSelectedVersionIndex(index);
}

void TechnicViewModel::installSelected(const QString& instanceName, const QString& groupName)
{
    if (m_selectedPackIndex < 0) {
        emit installFinished(false, tr("No pack selected"));
        return;
    }

    if (m_currentPack.broken || m_currentPack.url.isEmpty()) {
        emit installFinished(false, tr("Pack is broken or missing URL"));
        return;
    }

    emit installStarted();

    InstanceTask* installTask = nullptr;
    QString version = m_currentPack.currentVersion;

    // If user selected a specific version and it's a solder pack
    if (m_selectedVersionIndex >= 0 && m_selectedVersionIndex < m_selectedPackVersions.size()) {
        version = m_selectedPackVersions.at(m_selectedVersionIndex);
    }

    if (!m_currentPack.isSolder) {
        installTask = new Technic::SingleZipPackInstallTask(m_currentPack.url, m_currentPack.minecraftVersion);
    } else {
        installTask = new Technic::SolderPackInstallTask(APPLICATION->network(), m_currentPack.url, m_currentPack.slug, version,
                                                         m_currentPack.minecraftVersion);
    }

    installTask->setName(instanceName.isEmpty() ? m_currentPack.name : instanceName);
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

void TechnicViewModel::clearSelection()
{
    m_selectedPackIndex = -1;
    m_selectedVersionIndex = -1;
    m_selectedPack.clear();
    m_selectedPackVersions.clear();
    m_currentPack = {};

    emit selectedPackIndexChanged();
    emit selectedPackChanged();
    emit selectedPackVersionsChanged();
    emit selectedVersionIndexChanged();
}

QVariantMap TechnicViewModel::getPackInfo(int index) const
{
    if (index < 0 || index >= m_listModel->rowCount(QModelIndex()))
        return {};

    auto modelIndex = m_listModel->index(index, 0);
    QVariantMap info;
    info["name"] = modelIndex.data(Qt::DisplayRole).toString();
    info["description"] = modelIndex.data(Qt::ToolTipRole).toString();
    info["icon"] = modelIndex.data(Qt::DecorationRole);
    return info;
}

void TechnicViewModel::onSearchFinished()
{
    m_isLoading = false;
    m_statusMessage = tr("Search complete - %1 results").arg(m_listModel->rowCount(QModelIndex()));
    emit isLoadingChanged();
    emit statusMessageChanged();
    emit hasActiveSearchChanged();
}

void TechnicViewModel::updateSelectedPackInfo()
{
    if (m_selectedPackIndex < 0 || m_selectedPackIndex >= m_listModel->rowCount(QModelIndex())) {
        m_selectedPack.clear();
        m_selectedPackVersions.clear();
        m_selectedVersionIndex = -1;
        m_currentPack = {};
        return;
    }

    auto modelIndex = m_listModel->index(m_selectedPackIndex, 0);
    auto data = modelIndex.data(Qt::UserRole);

    if (data.canConvert<Technic::Modpack>()) {
        m_currentPack = data.value<Technic::Modpack>();
        m_selectedPack["name"] = m_currentPack.name;
        m_selectedPack["slug"] = m_currentPack.slug;
        m_selectedPack["description"] = m_currentPack.description;
        m_selectedPack["author"] = m_currentPack.author;
        m_selectedPack["websiteUrl"] = m_currentPack.websiteUrl;
        m_selectedPack["minecraftVersion"] = m_currentPack.minecraftVersion;
        m_selectedPack["isSolder"] = m_currentPack.isSolder;
        m_selectedPack["currentVersion"] = m_currentPack.currentVersion;

        // Load metadata if not already loaded
        if (!m_currentPack.metadataLoaded) {
            loadPackMetadata();
        } else {
            // Update versions
            m_selectedPackVersions.clear();
            if (m_currentPack.isSolder && m_currentPack.versionsLoaded) {
                for (auto i = m_currentPack.versions.size(); i--;) {
                    m_selectedPackVersions.append(m_currentPack.versions.at(i));
                }
                if (!m_selectedPackVersions.isEmpty()) {
                    int recIdx = m_selectedPackVersions.indexOf(m_currentPack.recommended);
                    m_selectedVersionIndex = (recIdx >= 0) ? recIdx : 0;
                }
            } else {
                m_selectedPackVersions.append(m_currentPack.currentVersion);
                m_selectedVersionIndex = 0;
            }
            emit selectedPackVersionsChanged();
            emit selectedVersionIndexChanged();
        }
    } else {
        // Fallback to display role data
        m_selectedPack["name"] = modelIndex.data(Qt::DisplayRole).toString();
        m_selectedPack["description"] = modelIndex.data(Qt::ToolTipRole).toString();
    }
}

void TechnicViewModel::loadPackMetadata()
{
    if (m_currentPack.slug.isEmpty())
        return;

    m_isLoadingMetadata = true;
    emit isLoadingMetadataChanged();

    m_metadataJob = makeShared<NetJob>(QString("Technic::PackMeta(%1)").arg(m_currentPack.name), APPLICATION->network());
    QString slug = m_currentPack.slug;
    m_metadataJob->addNetAction(Net::ApiDownload::makeByteArray(
        QString("%1modpack/%2?build=%3").arg(BuildConfig.TECHNIC_API_BASE_URL, slug, BuildConfig.TECHNIC_API_BUILD), m_response));

    connect(m_metadataJob.get(), &NetJob::succeeded, this, &TechnicViewModel::onMetadataLoaded);
    connect(m_metadataJob.get(), &NetJob::failed, this, [this](const QString& reason) {
        m_isLoadingMetadata = false;
        emit isLoadingMetadataChanged();
        m_statusMessage = tr("Failed to load pack info: %1").arg(reason);
        emit statusMessageChanged();
    });

    m_metadataJob->start();
}

void TechnicViewModel::onMetadataLoaded()
{
    m_metadataJob.reset();

    QJsonParseError parse_error{};
    QJsonDocument doc = QJsonDocument::fromJson(*m_response, &parse_error);
    QJsonObject obj = doc.object();

    if (parse_error.error != QJsonParseError::NoError) {
        m_isLoadingMetadata = false;
        emit isLoadingMetadataChanged();
        return;
    }

    if (!obj.contains("url")) {
        m_isLoadingMetadata = false;
        emit isLoadingMetadataChanged();
        return;
    }

    QJsonValueRef url = obj["url"];
    if (url.isString()) {
        m_currentPack.url = url.toString();
    } else {
        if (obj.contains("solder")) {
            QJsonValueRef solderUrl = obj["solder"];
            if (solderUrl.isString()) {
                m_currentPack.url = solderUrl.toString();
                m_currentPack.isSolder = true;
            }
        }
    }

    m_currentPack.minecraftVersion = Json::ensureString(obj, "minecraft", QString(), "__placeholder__");
    m_currentPack.websiteUrl = Json::ensureString(obj, "platformUrl", QString(), "__placeholder__");
    m_currentPack.author = Json::ensureString(obj, "user", QString(), "__placeholder__");
    m_currentPack.description = Json::ensureString(obj, "description", QString(), "__placeholder__");
    m_currentPack.currentVersion = Json::ensureString(obj, "version", QString(), "__placeholder__");
    m_currentPack.metadataLoaded = true;

    // Update the selectedPack map
    m_selectedPack["description"] = m_currentPack.description;
    m_selectedPack["author"] = m_currentPack.author;
    m_selectedPack["websiteUrl"] = m_currentPack.websiteUrl;
    m_selectedPack["minecraftVersion"] = m_currentPack.minecraftVersion;
    m_selectedPack["isSolder"] = m_currentPack.isSolder;
    m_selectedPack["currentVersion"] = m_currentPack.currentVersion;
    emit selectedPackChanged();

    // Strip trailing forward-slashes from Solder URL's
    if (m_currentPack.isSolder) {
        while (m_currentPack.url.endsWith('/'))
            m_currentPack.url.chop(1);

        loadSolderVersions();
    } else {
        m_selectedPackVersions.clear();
        m_selectedPackVersions.append(m_currentPack.currentVersion);
        m_selectedVersionIndex = 0;
        emit selectedPackVersionsChanged();
        emit selectedVersionIndexChanged();

        m_isLoadingMetadata = false;
        emit isLoadingMetadataChanged();
    }
}

void TechnicViewModel::loadSolderVersions()
{
    m_metadataJob = makeShared<NetJob>(QString("Technic::SolderMeta(%1)").arg(m_currentPack.name), APPLICATION->network());
    auto url = QString("%1/modpack/%2").arg(m_currentPack.url, m_currentPack.slug);
    m_metadataJob->addNetAction(Net::ApiDownload::makeByteArray(QUrl(url), m_response));

    connect(m_metadataJob.get(), &NetJob::succeeded, this, &TechnicViewModel::onSolderLoaded);
    connect(m_metadataJob.get(), &NetJob::failed, this, [this](const QString&) {
        // Fallback to current version
        m_selectedPackVersions.clear();
        m_selectedPackVersions.append(m_currentPack.currentVersion);
        m_selectedVersionIndex = 0;
        emit selectedPackVersionsChanged();
        emit selectedVersionIndexChanged();

        m_isLoadingMetadata = false;
        emit isLoadingMetadataChanged();
    });

    m_metadataJob->start();
}

void TechnicViewModel::onSolderLoaded()
{
    m_metadataJob.reset();

    QJsonParseError parse_error{};
    auto doc = QJsonDocument::fromJson(*m_response, &parse_error);

    if (parse_error.error != QJsonParseError::NoError) {
        m_selectedPackVersions.clear();
        m_selectedPackVersions.append(m_currentPack.currentVersion);
        m_selectedVersionIndex = 0;
        emit selectedPackVersionsChanged();
        emit selectedVersionIndexChanged();

        m_isLoadingMetadata = false;
        emit isLoadingMetadataChanged();
        return;
    }

    auto obj = doc.object();
    TechnicSolder::Pack pack;
    try {
        TechnicSolder::loadPack(pack, obj);
    } catch (const JSONValidationError&) {
        m_selectedPackVersions.clear();
        m_selectedPackVersions.append(m_currentPack.currentVersion);
        m_selectedVersionIndex = 0;
        emit selectedPackVersionsChanged();
        emit selectedVersionIndexChanged();

        m_isLoadingMetadata = false;
        emit isLoadingMetadataChanged();
        return;
    }

    m_currentPack.versionsLoaded = true;
    m_currentPack.recommended = pack.recommended;
    m_currentPack.versions = pack.builds;

    m_selectedPackVersions.clear();
    // Reverse order so newest versions are first
    for (auto i = m_currentPack.versions.size(); i--;) {
        m_selectedPackVersions.append(m_currentPack.versions.at(i));
    }

    if (!m_selectedPackVersions.isEmpty()) {
        int recIdx = m_selectedPackVersions.indexOf(m_currentPack.recommended);
        m_selectedVersionIndex = (recIdx >= 0) ? recIdx : 0;
    }

    emit selectedPackVersionsChanged();
    emit selectedVersionIndexChanged();

    m_isLoadingMetadata = false;
    emit isLoadingMetadataChanged();
}
