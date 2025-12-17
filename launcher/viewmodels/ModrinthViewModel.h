// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2025 Project Tick

#pragma once

#include <QAbstractListModel>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QVariantMap>

#include "modplatform/ModIndex.h"
#include "modplatform/modrinth/ModrinthAPI.h"
#include "net/NetJob.h"

class ModrinthPackListModel;

class ModrinthViewModel : public QObject {
    Q_OBJECT

    Q_PROPERTY(QAbstractItemModel* packsModel READ packsModel CONSTANT)
    Q_PROPERTY(bool isLoading READ isLoading NOTIFY isLoadingChanged)
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY statusMessageChanged)
    Q_PROPERTY(QString searchTerm READ searchTerm WRITE setSearchTerm NOTIFY searchTermChanged)
    Q_PROPERTY(int sortIndex READ sortIndex WRITE setSortIndex NOTIFY sortIndexChanged)
    Q_PROPERTY(QStringList sortOptions READ sortOptions CONSTANT)
    Q_PROPERTY(QStringList categories READ categories NOTIFY categoriesChanged)
    Q_PROPERTY(int selectedCategoryIndex READ selectedCategoryIndex WRITE setSelectedCategoryIndex NOTIFY selectedCategoryIndexChanged)
    Q_PROPERTY(QStringList loaders READ loaders CONSTANT)
    Q_PROPERTY(int selectedLoaderIndex READ selectedLoaderIndex WRITE setSelectedLoaderIndex NOTIFY selectedLoaderIndexChanged)

    // Selected pack
    Q_PROPERTY(int selectedPackIndex READ selectedPackIndex NOTIFY selectedPackIndexChanged)
    Q_PROPERTY(QVariantMap selectedPack READ selectedPack NOTIFY selectedPackChanged)
    Q_PROPERTY(QStringList selectedPackVersions READ selectedPackVersions NOTIFY selectedPackVersionsChanged)
    Q_PROPERTY(int selectedVersionIndex READ selectedVersionIndex WRITE setSelectedVersionIndex NOTIFY selectedVersionIndexChanged)

   public:
    explicit ModrinthViewModel(QObject* parent = nullptr);
    ~ModrinthViewModel() override;

    QAbstractItemModel* packsModel() const;
    bool isLoading() const { return m_isLoading; }
    QString statusMessage() const { return m_statusMessage; }
    QString searchTerm() const { return m_searchTerm; }
    void setSearchTerm(const QString& term);
    int sortIndex() const { return m_sortIndex; }
    void setSortIndex(int index);
    QStringList sortOptions() const;
    QStringList categories() const { return m_categories; }
    int selectedCategoryIndex() const { return m_selectedCategoryIndex; }
    void setSelectedCategoryIndex(int index);
    QStringList loaders() const;
    int selectedLoaderIndex() const { return m_selectedLoaderIndex; }
    void setSelectedLoaderIndex(int index);

    int selectedPackIndex() const { return m_selectedPackIndex; }
    QVariantMap selectedPack() const { return m_selectedPack; }
    QStringList selectedPackVersions() const { return m_selectedPackVersions; }
    int selectedVersionIndex() const { return m_selectedVersionIndex; }
    void setSelectedVersionIndex(int index);

   public slots:
    void refresh();
    void search(const QString& term);
    void selectPack(int index);
    void selectVersion(int index);
    void installSelected(const QString& instanceName, const QString& groupName);
    void clearSelection();

   signals:
    void isLoadingChanged();
    void statusMessageChanged();
    void searchTermChanged();
    void sortIndexChanged();
    void categoriesChanged();
    void selectedCategoryIndexChanged();
    void selectedLoaderIndexChanged();
    void selectedPackIndexChanged();
    void selectedPackChanged();
    void selectedPackVersionsChanged();
    void selectedVersionIndexChanged();
    void installStarted(const QString& packName);
    void installCompleted(const QString& packName);
    void installFailed(const QString& packName, const QString& error);

   private slots:
    void onSearchFinished();
    void onVersionsLoaded();
    void onExtraInfoLoaded();
    void triggerSearch();
    void loadCategories();

   private:
    void setLoading(bool loading);
    void setStatusMessage(const QString& message);
    void updateSelectedPackInfo();
    void loadVersionsForPack();
    void loadExtraInfoForPack();

    ModrinthPackListModel* m_model = nullptr;

    bool m_isLoading = false;
    QString m_statusMessage;
    QString m_searchTerm;
    int m_sortIndex = 0;
    QStringList m_categories;
    int m_selectedCategoryIndex = 0;
    int m_selectedLoaderIndex = 0;

    int m_selectedPackIndex = -1;
    QVariantMap m_selectedPack;
    QStringList m_selectedPackVersions;
    int m_selectedVersionIndex = -1;

    ModPlatform::IndexedPack::Ptr m_currentPack;
    QTimer m_searchTimer;
    Task::Ptr m_versionsJob;
    Task::Ptr m_infoJob;
};

// Internal model for QML
class ModrinthPackListModel : public QAbstractListModel {
    Q_OBJECT

   public:
    enum Roles { NameRole = Qt::UserRole + 1, DescriptionRole, AuthorRole, IconUrlRole, DownloadsRole, FollowsRole, PackDataRole };

    explicit ModrinthPackListModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    bool canFetchMore(const QModelIndex& parent) const override;
    void fetchMore(const QModelIndex& parent) override;

    void searchWithTerm(const QString& term, int sort);
    ModPlatform::IndexedPack::Ptr packAt(int index) const;
    void clear();

    bool hasActiveSearch() const { return m_jobPtr && m_jobPtr->isRunning(); }
    Task::Ptr activeSearchJob() { return hasActiveSearch() ? m_jobPtr : nullptr; }

   signals:
    void searchFinished();
    void searchFailed(const QString& reason);

   private slots:
    void searchRequestFinished(QList<ModPlatform::IndexedPack::Ptr>& packs);
    void searchRequestFailed(const QString& reason);

   private:
    void performPaginatedSearch();

    QList<ModPlatform::IndexedPack::Ptr> m_packs;
    QString m_currentSearchTerm;
    int m_currentSort = 0;
    int m_nextSearchOffset = 0;
    enum SearchState { None, CanPossiblyFetchMore, ResetRequested, Finished } m_searchState = None;
    Task::Ptr m_jobPtr;
};
