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

#pragma once

#include <QAbstractListModel>
#include <QObject>
#include <QString>
#include <QVariantMap>
#include <memory>

#include "net/NetJob.h"
#include "ui/pages/modplatform/technic/TechnicModel.h"

class TechnicViewModel : public QObject {
    Q_OBJECT

    Q_PROPERTY(QAbstractItemModel* packsModel READ packsModel CONSTANT)
    Q_PROPERTY(bool isLoading READ isLoading NOTIFY isLoadingChanged)
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY statusMessageChanged)
    Q_PROPERTY(QString searchTerm READ searchTerm WRITE setSearchTerm NOTIFY searchTermChanged)
    Q_PROPERTY(int selectedPackIndex READ selectedPackIndex WRITE setSelectedPackIndex NOTIFY selectedPackIndexChanged)
    Q_PROPERTY(QVariantMap selectedPack READ selectedPack NOTIFY selectedPackChanged)
    Q_PROPERTY(QStringList selectedPackVersions READ selectedPackVersions NOTIFY selectedPackVersionsChanged)
    Q_PROPERTY(int selectedVersionIndex READ selectedVersionIndex WRITE setSelectedVersionIndex NOTIFY selectedVersionIndexChanged)
    Q_PROPERTY(bool hasActiveSearch READ hasActiveSearch NOTIFY hasActiveSearchChanged)
    Q_PROPERTY(bool isLoadingMetadata READ isLoadingMetadata NOTIFY isLoadingMetadataChanged)

   public:
    explicit TechnicViewModel(QObject* parent = nullptr);
    ~TechnicViewModel();

    // Properties
    QAbstractItemModel* packsModel() const;
    bool isLoading() const;
    QString statusMessage() const;
    QString searchTerm() const;
    void setSearchTerm(const QString& term);
    int selectedPackIndex() const;
    void setSelectedPackIndex(int index);
    QVariantMap selectedPack() const;
    QStringList selectedPackVersions() const;
    int selectedVersionIndex() const;
    void setSelectedVersionIndex(int index);
    bool hasActiveSearch() const;
    bool isLoadingMetadata() const;

    // Actions
    Q_INVOKABLE void refresh();
    Q_INVOKABLE void search(const QString& term);
    Q_INVOKABLE void selectPack(int index);
    Q_INVOKABLE void selectVersion(int index);
    Q_INVOKABLE void installSelected(const QString& instanceName, const QString& groupName);
    Q_INVOKABLE void clearSelection();
    Q_INVOKABLE QVariantMap getPackInfo(int index) const;

   signals:
    void isLoadingChanged();
    void statusMessageChanged();
    void searchTermChanged();
    void selectedPackIndexChanged();
    void selectedPackChanged();
    void selectedPackVersionsChanged();
    void selectedVersionIndexChanged();
    void hasActiveSearchChanged();
    void isLoadingMetadataChanged();
    void installStarted();
    void installFinished(bool success, const QString& message);

   private slots:
    void onSearchFinished();
    void onMetadataLoaded();
    void onSolderLoaded();

   private:
    void updateSelectedPackInfo();
    void loadPackMetadata();
    void loadSolderVersions();

    Technic::ListModel* m_listModel = nullptr;
    Technic::Modpack m_currentPack;
    NetJob::Ptr m_metadataJob;
    std::shared_ptr<QByteArray> m_response = std::make_shared<QByteArray>();

    bool m_isLoading = false;
    bool m_isLoadingMetadata = false;
    QString m_statusMessage;
    QString m_searchTerm;
    int m_selectedPackIndex = -1;
    QVariantMap m_selectedPack;
    QStringList m_selectedPackVersions;
    int m_selectedVersionIndex = -1;
};
