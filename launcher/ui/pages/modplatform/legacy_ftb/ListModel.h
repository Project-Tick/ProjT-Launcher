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
 */
#pragma once

#include <RWStorage.h>
#include <modplatform/legacy_ftb/PackHelpers.h>

#include <QAbstractListModel>
#include <QIcon>
#include <QSortFilterProxyModel>
#include <QStyledItemDelegate>
#include <QThreadPool>

#include <functional>

namespace LegacyFTB {

using FTBLogoMap = QMap<QString, QIcon>;
using LogoCallback = std::function<void(QString)>;

class FilterModel : public QSortFilterProxyModel {
    Q_OBJECT
   public:
    FilterModel(QObject* parent = Q_NULLPTR);
    enum Sorting { ByName, ByGameVersion };
    const QMap<QString, Sorting> getAvailableSortings();
    QString translateCurrentSorting();
    void setSorting(Sorting sorting);
    Sorting getCurrentSorting();
    void setSearchTerm(QString term);

   protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;
    bool lessThan(const QModelIndex& left, const QModelIndex& right) const override;

   private:
    QMap<QString, Sorting> sortings;
    Sorting currentSorting;
    QString searchTerm;
};

class ListModel : public QAbstractListModel {
    Q_OBJECT
   private:
    ModpackList modpacks;
    QStringList m_failedLogos;
    QStringList m_loadingLogos;
    FTBLogoMap m_logoMap;
    QMap<QString, LogoCallback> waitingCallbacks;

    void requestLogo(QString file);
    QString translatePackType(PackType type) const;

   private slots:
    void logoFailed(QString logo);
    void logoLoaded(QString logo, QIcon out);

   public:
    ListModel(QObject* parent);
    ~ListModel();
    int rowCount(const QModelIndex& parent) const override;
    int columnCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    void fill(ModpackList modpacks);
    void addPack(const Modpack& modpack);
    void clear();
    void remove(int row);

    Modpack at(int row);
    void getLogo(const QString& logo, LogoCallback callback);
};

}  // namespace LegacyFTB
