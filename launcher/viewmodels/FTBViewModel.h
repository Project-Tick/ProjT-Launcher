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

#pragma once

#include <QAbstractListModel>
#include <QObject>
#include <QSortFilterProxyModel>
#include <QString>
#include <QVariantList>
#include <QVariantMap>
#include <memory>

#include "ui/pages/modplatform/legacy_ftb/ListModel.h"

// Forward declaration
namespace LegacyFTB {
class PackFetchTask;
class PrivatePackManager;
}  // namespace LegacyFTB

class FTBViewModel : public QObject {
    Q_OBJECT

    Q_PROPERTY(QAbstractItemModel* packsModel READ packsModel CONSTANT)
    Q_PROPERTY(QAbstractItemModel* privatePacksModel READ privatePacksModel CONSTANT)
    Q_PROPERTY(bool isLoading READ isLoading NOTIFY isLoadingChanged)
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY statusMessageChanged)
    Q_PROPERTY(QString searchTerm READ searchTerm WRITE setSearchTerm NOTIFY searchTermChanged)
    Q_PROPERTY(int selectedPackIndex READ selectedPackIndex WRITE setSelectedPackIndex NOTIFY selectedPackIndexChanged)
    Q_PROPERTY(QVariantMap selectedPack READ selectedPack NOTIFY selectedPackChanged)
    Q_PROPERTY(QStringList selectedPackVersions READ selectedPackVersions NOTIFY selectedPackVersionsChanged)
    Q_PROPERTY(int selectedVersionIndex READ selectedVersionIndex WRITE setSelectedVersionIndex NOTIFY selectedVersionIndexChanged)
    Q_PROPERTY(bool showingPrivatePacks READ showingPrivatePacks WRITE setShowingPrivatePacks NOTIFY showingPrivatePacksChanged)

   public:
    explicit FTBViewModel(QObject* parent = nullptr);
    ~FTBViewModel();

    // Properties
    QAbstractItemModel* packsModel() const;
    QAbstractItemModel* privatePacksModel() const;
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
    bool showingPrivatePacks() const;
    void setShowingPrivatePacks(bool showing);

    // Actions
    Q_INVOKABLE void refresh();
    Q_INVOKABLE void search(const QString& term);
    Q_INVOKABLE void selectPack(int index);
    Q_INVOKABLE void selectVersion(int index);
    Q_INVOKABLE void installSelected(const QString& instanceName, const QString& groupName);
    Q_INVOKABLE void clearSelection();
    Q_INVOKABLE void addPrivatePack(const QString& packCode);
    Q_INVOKABLE void removePrivatePack(const QString& packCode);
    Q_INVOKABLE QVariantMap getPackInfo(int index) const;

   signals:
    void isLoadingChanged();
    void statusMessageChanged();
    void searchTermChanged();
    void selectedPackIndexChanged();
    void selectedPackChanged();
    void selectedPackVersionsChanged();
    void selectedVersionIndexChanged();
    void showingPrivatePacksChanged();
    void installStarted();
    void installFinished(bool success, const QString& message);
    void privatePackAdded(bool success, const QString& message);

   private slots:
    void onPublicPacksLoaded(const LegacyFTB::ModpackList& packs);
    void onPrivatePacksLoaded(const LegacyFTB::ModpackList& packs);
    void onPackFetchFailed(const QString& error);
    void onPackFetchFinished(const LegacyFTB::ModpackList& publicPacks, const LegacyFTB::ModpackList& thirdPartyPacks);
    void onPrivatePackFetched(const LegacyFTB::Modpack& pack);
    void onPrivatePackFetchFailed(const QString& error, const QString& packCode);
    void onPackFetchAborted();

   private:
    void loadPacks();
    void updateSelectedPackInfo();
    QAbstractItemModel* currentModel() const;
    LegacyFTB::Modpack resolveSelectedPack() const;

    LegacyFTB::ListModel* m_publicPacksModel = nullptr;
    LegacyFTB::FilterModel* m_publicFilterModel = nullptr;
    LegacyFTB::ListModel* m_privatePacksModel = nullptr;
    LegacyFTB::FilterModel* m_privateFilterModel = nullptr;
    std::shared_ptr<LegacyFTB::PackFetchTask> m_fetchTask;
    std::unique_ptr<LegacyFTB::PrivatePackManager> m_privatePackManager;

    bool m_isLoading = false;
    QString m_statusMessage;
    QString m_searchTerm;
    int m_selectedPackIndex = -1;
    QVariantMap m_selectedPack;
    QStringList m_selectedPackVersions;
    int m_selectedVersionIndex = -1;
    bool m_showingPrivatePacks = false;
};
