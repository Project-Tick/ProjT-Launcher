// SPDX-License-Identifier: GPL-3.0-only
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
#include <QVariantMap>

#include "modplatform/atlauncher/ATLPackIndex.h"
#include "ui/pages/modplatform/atlauncher/AtlFilterModel.h"
#include "ui/pages/modplatform/atlauncher/AtlListModel.h"

class ATLauncherViewModel : public QObject {
    Q_OBJECT

    Q_PROPERTY(QAbstractItemModel* packsModel READ packsModel CONSTANT)
    Q_PROPERTY(bool isLoading READ isLoading NOTIFY isLoadingChanged)
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY statusMessageChanged)
    Q_PROPERTY(QString searchTerm READ searchTerm WRITE setSearchTerm NOTIFY searchTermChanged)
    Q_PROPERTY(QString selectedMinecraftVersion READ selectedMinecraftVersion WRITE setSelectedMinecraftVersion NOTIFY
                   selectedMinecraftVersionChanged)
    Q_PROPERTY(QStringList minecraftVersions READ minecraftVersions NOTIFY minecraftVersionsChanged)
    Q_PROPERTY(int selectedPackIndex READ selectedPackIndex WRITE setSelectedPackIndex NOTIFY selectedPackIndexChanged)
    Q_PROPERTY(QVariantMap selectedPack READ selectedPack NOTIFY selectedPackChanged)
    Q_PROPERTY(QStringList selectedPackVersions READ selectedPackVersions NOTIFY selectedPackVersionsChanged)
    Q_PROPERTY(int selectedVersionIndex READ selectedVersionIndex WRITE setSelectedVersionIndex NOTIFY selectedVersionIndexChanged)

   public:
    explicit ATLauncherViewModel(QObject* parent = nullptr);
    ~ATLauncherViewModel();

    // Properties
    QAbstractItemModel* packsModel() const;
    bool isLoading() const;
    QString statusMessage() const;
    QString searchTerm() const;
    void setSearchTerm(const QString& term);
    QString selectedMinecraftVersion() const;
    void setSelectedMinecraftVersion(const QString& version);
    QStringList minecraftVersions() const;
    int selectedPackIndex() const;
    void setSelectedPackIndex(int index);
    QVariantMap selectedPack() const;
    QStringList selectedPackVersions() const;
    int selectedVersionIndex() const;
    void setSelectedVersionIndex(int index);

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
    void selectedMinecraftVersionChanged();
    void minecraftVersionsChanged();
    void selectedPackIndexChanged();
    void selectedPackChanged();
    void selectedPackVersionsChanged();
    void selectedVersionIndexChanged();
    void installStarted();
    void installFinished(bool success, const QString& message);

   private slots:
    void onPacksLoaded();
    void onLoadError(const QString& error);
    void onSelectionChanged();

   private:
    void loadPacks();
    void updateMinecraftVersions();
    void updateSelectedPackInfo();

    Atl::ListModel* m_listModel = nullptr;
    Atl::FilterModel* m_filterModel = nullptr;

    bool m_isLoading = false;
    QString m_statusMessage;
    QString m_searchTerm;
    QString m_selectedMinecraftVersion;
    QStringList m_minecraftVersions;
    int m_selectedPackIndex = -1;
    QVariantMap m_selectedPack;
    QStringList m_selectedPackVersions;
    int m_selectedVersionIndex = -1;
};
