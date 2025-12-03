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

#include "AccountsViewModel.h"

#include "Application.h"
#include "minecraft/auth/AccountList.h"
#include "minecraft/auth/MinecraftAccount.h"

AccountsViewModel::AccountsViewModel(QObject* parent)
    : QObject(parent)
{
    m_accountList = APPLICATION->accounts().get();
    
    if (m_accountList) {
        connect(m_accountList, &AccountList::listChanged, 
                this, &AccountsViewModel::onListChanged);
        connect(m_accountList, &AccountList::defaultAccountChanged, 
                this, &AccountsViewModel::onDefaultAccountChanged);
        connect(m_accountList, &AccountList::activityChanged, 
                this, &AccountsViewModel::onActivityChanged);
    }
}

int AccountsViewModel::count() const
{
    return m_accountList ? m_accountList->count() : 0;
}

int AccountsViewModel::defaultAccountIndex() const
{
    if (!m_accountList) return -1;
    
    auto defaultAcc = m_accountList->defaultAccount();
    if (!defaultAcc) return -1;
    
    for (int i = 0; i < m_accountList->count(); ++i) {
        if (m_accountList->at(i) == defaultAcc) {
            return i;
        }
    }
    return -1;
}

QString AccountsViewModel::defaultAccountName() const
{
    if (!m_accountList) return QString();
    
    auto defaultAcc = m_accountList->defaultAccount();
    return defaultAcc ? defaultAcc->profileName() : QString();
}

QString AccountsViewModel::defaultAccountProfileId() const
{
    if (!m_accountList) return QString();
    
    auto defaultAcc = m_accountList->defaultAccount();
    return defaultAcc ? defaultAcc->profileId() : QString();
}

QString AccountsViewModel::defaultAccountType() const
{
    if (!m_accountList) return QString();
    
    auto defaultAcc = m_accountList->defaultAccount();
    return defaultAcc ? defaultAcc->typeString() : QString();
}

bool AccountsViewModel::hasAccounts() const
{
    return count() > 0;
}

bool AccountsViewModel::isActive() const
{
    return m_accountList ? m_accountList->isActive() : false;
}

QAbstractListModel* AccountsViewModel::model() const
{
    return m_accountList;
}

void AccountsViewModel::addMicrosoftAccount()
{
    if (!m_accountList) return;
    
    auto account = MinecraftAccount::createBlankMSA();
    m_currentLoginTask = account->login(false);
    
    if (m_currentLoginTask) {
        emit loginStarted();
        
        connect(m_currentLoginTask.get(), &AuthFlow::succeeded, 
                this, [this, account]() {
                    m_accountList->addAccount(account);
                    m_currentLoginTask.reset();
                    emit loginFinished(true, tr("Account added successfully"));
                });
        
        connect(m_currentLoginTask.get(), &AuthFlow::failed, 
                this, [this](const QString& reason) {
                    m_currentLoginTask.reset();
                    emit loginFinished(false, reason);
                });
        
        // Handle OAuth URL for browser login
        connect(m_currentLoginTask.get(), &AuthFlow::authorizeWithBrowser, 
                this, [this](const QUrl& url) {
                    emit loginUrlReady(url.toString(), QString());
                });
    }
}

void AccountsViewModel::addMicrosoftAccountWithDevice()
{
    if (!m_accountList) return;
    
    auto account = MinecraftAccount::createBlankMSA();
    m_currentLoginTask = account->login(true);  // Use device code flow
    
    if (m_currentLoginTask) {
        emit loginStarted();
        
        connect(m_currentLoginTask.get(), &AuthFlow::succeeded, 
                this, [this, account]() {
                    m_accountList->addAccount(account);
                    m_currentLoginTask.reset();
                    emit loginFinished(true, tr("Account added successfully"));
                });
        
        connect(m_currentLoginTask.get(), &AuthFlow::failed, 
                this, [this](const QString& reason) {
                    m_currentLoginTask.reset();
                    emit loginFinished(false, reason);
                });
        
        // Handle device code flow - uses authorizeWithBrowserWithExtra signal
        connect(m_currentLoginTask.get(), &AuthFlow::authorizeWithBrowserWithExtra, 
                this, [this](const QString& url, const QString& code, int) {
                    emit loginUrlReady(url, code);
                });
    }
}

void AccountsViewModel::addOfflineAccount(const QString& username)
{
    if (!m_accountList || username.isEmpty()) return;
    
    auto account = MinecraftAccount::createOffline(username);
    m_accountList->addAccount(account);
    
    emit loginFinished(true, tr("Offline account added"));
}

void AccountsViewModel::removeAccount(int index)
{
    if (!m_accountList || index < 0 || index >= m_accountList->count()) return;
    
    m_accountList->removeAccount(m_accountList->index(index, 0));
    emit accountRemoved(index);
}

void AccountsViewModel::setDefaultAccount(int index)
{
    if (!m_accountList || index < 0 || index >= m_accountList->count()) return;
    
    m_accountList->setDefaultAccount(m_accountList->at(index));
}

void AccountsViewModel::refreshAccount(int index)
{
    if (!m_accountList || index < 0 || index >= m_accountList->count()) return;
    
    auto account = m_accountList->at(index);
    if (account) {
        m_accountList->requestRefresh(account->internalId());
    }
}

void AccountsViewModel::refreshAllAccounts()
{
    if (!m_accountList) return;
    
    for (int i = 0; i < m_accountList->count(); ++i) {
        auto account = m_accountList->at(i);
        if (account && account->accountType() != AccountType::Offline) {
            m_accountList->queueRefresh(account->internalId());
        }
    }
}

QVariantMap AccountsViewModel::getAccountInfo(int index) const
{
    QVariantMap info;
    
    if (!m_accountList || index < 0 || index >= m_accountList->count()) {
        return info;
    }
    
    auto account = m_accountList->at(index);
    if (account) {
        info["name"] = account->profileName();
        info["type"] = account->typeString();
        info["profileId"] = account->profileId();
        info["isDefault"] = isAccountDefault(index);
        info["isExpired"] = isAccountExpired(index);
        info["hasProfile"] = account->hasProfile();
        info["ownsMinecraft"] = account->ownsMinecraft();
        
        switch (account->accountState()) {
            case AccountState::Unchecked: info["status"] = "unchecked"; break;
            case AccountState::Offline: info["status"] = "offline"; break;
            case AccountState::Working: info["status"] = "working"; break;
            case AccountState::Online: info["status"] = "online"; break;
            case AccountState::Disabled: info["status"] = "disabled"; break;
            case AccountState::Errored: info["status"] = "error"; break;
            case AccountState::Expired: info["status"] = "expired"; break;
            case AccountState::Gone: info["status"] = "gone"; break;
        }
    }
    
    return info;
}

QString AccountsViewModel::getAccountName(int index) const
{
    if (!m_accountList || index < 0 || index >= m_accountList->count()) {
        return QString();
    }
    
    auto account = m_accountList->at(index);
    return account ? account->profileName() : QString();
}

QString AccountsViewModel::getAccountType(int index) const
{
    if (!m_accountList || index < 0 || index >= m_accountList->count()) {
        return QString();
    }
    
    auto account = m_accountList->at(index);
    return account ? account->typeString() : QString();
}

QString AccountsViewModel::getAccountStatus(int index) const
{
    if (!m_accountList || index < 0 || index >= m_accountList->count()) {
        return QString();
    }
    
    auto account = m_accountList->at(index);
    if (!account) return QString();
    
    switch (account->accountState()) {
        case AccountState::Unchecked: return tr("Unchecked");
        case AccountState::Offline: return tr("Offline");
        case AccountState::Working: return tr("Working...");
        case AccountState::Online: return tr("Online");
        case AccountState::Disabled: return tr("Disabled");
        case AccountState::Errored: return tr("Error");
        case AccountState::Expired: return tr("Expired");
        case AccountState::Gone: return tr("Gone");
        default: return tr("Unknown");
    }
}

QString AccountsViewModel::getAccountProfileId(int index) const
{
    if (!m_accountList || index < 0 || index >= m_accountList->count()) {
        return QString();
    }
    
    auto account = m_accountList->at(index);
    return account ? account->profileId() : QString();
}

bool AccountsViewModel::isAccountDefault(int index) const
{
    if (!m_accountList || index < 0 || index >= m_accountList->count()) {
        return false;
    }
    
    auto account = m_accountList->at(index);
    auto defaultAcc = m_accountList->defaultAccount();
    
    return account && defaultAcc && (account == defaultAcc);
}

bool AccountsViewModel::isAccountExpired(int index) const
{
    if (!m_accountList || index < 0 || index >= m_accountList->count()) {
        return false;
    }
    
    auto account = m_accountList->at(index);
    return account && account->shouldRefresh();
}

void AccountsViewModel::selectAccount(int index)
{
    setDefaultAccount(index);
}

void AccountsViewModel::cancelCurrentLogin()
{
    if (m_currentLoginTask) {
        // AuthFlow doesn't have a direct cancel, just disconnect and reset
        m_currentLoginTask.reset();
        emit loginFinished(false, tr("Login cancelled"));
    }
}

void AccountsViewModel::onListChanged()
{
    emit countChanged();
}

void AccountsViewModel::onDefaultAccountChanged()
{
    emit defaultAccountChanged();
}

void AccountsViewModel::onActivityChanged(bool active)
{
    Q_UNUSED(active)
    emit activityChanged();
}

void AccountsViewModel::onAuthSucceeded()
{
    emit loginFinished(true, tr("Login successful"));
    m_currentLoginTask.reset();
}

void AccountsViewModel::onAuthFailed(const QString& reason)
{
    emit loginFinished(false, reason);
    m_currentLoginTask.reset();
}
