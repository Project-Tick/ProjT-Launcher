// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team

#pragma once

#include <QApplication>
#include <QClipboard>
#include <QQmlPropertyMap>
#include <QVariantMap>
#include <QDir>
#include <QFile>

#include "BuildConfig.h"
#include "FileSystem.h"
#include "settings/SettingsObject.h"
#include "ui/dialogs/IconPickerDialog.h"

class QmlFileSystemBridge : public QObject {
    Q_OBJECT
   public:
    explicit QmlFileSystemBridge(QObject* parent = nullptr) : QObject(parent) {}

    Q_INVOKABLE bool supportsReflinks() const { return FS::canCloneOnFS(QDir::currentPath()); }
};

class QmlContextBridge : public QQmlPropertyMap {
    Q_OBJECT
    Q_PROPERTY(QVariantMap appInfo READ appInfo CONSTANT)
    Q_PROPERTY(QObject* fileSystem READ fileSystem CONSTANT)
    Q_PROPERTY(QObject* settings READ settingsObj CONSTANT)

   public:
    explicit QmlContextBridge(QObject* parent = nullptr) : QQmlPropertyMap(this, parent) {}

    void setSettings(SettingsObjectPtr settings) { m_settings = std::move(settings); }

    QVariantMap appInfo() const
    {
        QVariantMap info;
        info.insert(QStringLiteral("version"), BuildConfig.printableVersionString());
        info.insert(QStringLiteral("copyright"), BuildConfig.LAUNCHER_COPYRIGHT);
        info.insert(QStringLiteral("platform"), BuildConfig.BUILD_PLATFORM);
        info.insert(QStringLiteral("buildDate"), BuildConfig.BUILD_DATE);
        info.insert(QStringLiteral("gitCommit"), BuildConfig.GIT_COMMIT);
        info.insert(QStringLiteral("channel"), BuildConfig.VERSION_CHANNEL);
        info.insert(QStringLiteral("buildArtifact"), BuildConfig.BUILD_ARTIFACT);

        QFile creditsFile(":/documents/credits.html");
        if (creditsFile.open(QIODevice::ReadOnly)) {
            info.insert(QStringLiteral("credits"), QString::fromUtf8(creditsFile.readAll()));
        }
        return info;
    }

    QObject* fileSystem() { return &m_fileSystemBridge; }
    QObject* settingsObj() { return m_settings.get(); }

    Q_INVOKABLE void copyToClipboard(const QString& text)
    {
        if (auto clipboard = QApplication::clipboard()) {
            clipboard->setText(text);
        }
    }

    Q_INVOKABLE void showAboutQt() { QApplication::aboutQt(); }

    Q_INVOKABLE void showIconPicker()
    {
        IconPickerDialog dlg;
        dlg.exec();
    }

   private:
    SettingsObjectPtr m_settings;
    QmlFileSystemBridge m_fileSystemBridge;
};
