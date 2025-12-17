// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
 *
 *  This file is part of ProjT Launcher and is licensed under
 *  the GNU General Public License version 3 or later.
 *
 *  Accounts ViewModel for QML - manages Minecraft accounts
 */

#pragma once

#include <QAbstractListModel>
#include <QObject>
#include <QString>
#include <QVariantMap>

#include "minecraft/auth/AccountList.h"
#include "minecraft/auth/MinecraftAccount.h"

class AccountList;

class AccountsViewModel : public QObject {
    Q_OBJECT

    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(int defaultAccountIndex READ defaultAccountIndex NOTIFY defaultAccountChanged)
    Q_PROPERTY(QString defaultAccountName READ defaultAccountName NOTIFY defaultAccountChanged)
    Q_PROPERTY(QString defaultAccountProfileId READ defaultAccountProfileId NOTIFY defaultAccountChanged)
    Q_PROPERTY(QString defaultAccountType READ defaultAccountType NOTIFY defaultAccountChanged)
    Q_PROPERTY(bool hasAccounts READ hasAccounts NOTIFY countChanged)
    Q_PROPERTY(bool isActive READ isActive NOTIFY activityChanged)
    Q_PROPERTY(QAbstractListModel* model READ model CONSTANT)

   public:
    explicit AccountsViewModel(QObject* parent = nullptr);

    // Properties
    int count() const;
    int defaultAccountIndex() const;
    QString defaultAccountName() const;
    QString defaultAccountProfileId() const;
    QString defaultAccountType() const;
    bool hasAccounts() const;
    bool isActive() const;
    QAbstractListModel* model() const;

    // Account operations
    Q_INVOKABLE void addMicrosoftAccount();
    Q_INVOKABLE void addMicrosoftAccountWithDevice();
    Q_INVOKABLE void addOfflineAccount(const QString& username);
    Q_INVOKABLE void removeAccount(int index);
    Q_INVOKABLE void setDefaultAccount(int index);
    Q_INVOKABLE void refreshAccount(int index);
    Q_INVOKABLE void refreshAllAccounts();

    // Account info
    Q_INVOKABLE QVariantMap getAccountInfo(int index) const;
    Q_INVOKABLE QString getAccountName(int index) const;
    Q_INVOKABLE QString getAccountType(int index) const;
    Q_INVOKABLE QString getAccountStatus(int index) const;
    Q_INVOKABLE QString getAccountProfileId(int index) const;
    Q_INVOKABLE bool isAccountDefault(int index) const;
    Q_INVOKABLE bool isAccountExpired(int index) const;

    // Utility
    Q_INVOKABLE void selectAccount(int index);
    Q_INVOKABLE void cancelCurrentLogin();

   signals:
    void countChanged();
    void defaultAccountChanged();
    void activityChanged();
    void accountAdded(int index);
    void accountRemoved(int index);
    void accountRefreshed(int index, bool success);
    void loginStarted();
    void loginFinished(bool success, const QString& message);
    void loginUrlReady(const QString& url, const QString& code);

   private slots:
    void onListChanged();
    void onDefaultAccountChanged();
    void onActivityChanged(bool active);
    void onAuthSucceeded();
    void onAuthFailed(const QString& reason);

   private:
    AccountList* m_accountList = nullptr;
    shared_qobject_ptr<AuthFlow> m_currentLoginTask;
    int m_pendingAccountIndex = -1;
};
