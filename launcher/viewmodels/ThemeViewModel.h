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

class ThemeManager;

class ThemeViewModel : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString currentTheme READ currentTheme WRITE setCurrentTheme NOTIFY currentThemeChanged)
    Q_PROPERTY(QString currentIconTheme READ currentIconTheme WRITE setCurrentIconTheme NOTIFY currentIconThemeChanged)
    Q_PROPERTY(QAbstractListModel* availableThemes READ availableThemes CONSTANT)
    Q_PROPERTY(QAbstractListModel* availableIconThemes READ availableIconThemes CONSTANT)

    // Theme color properties for QML
    Q_PROPERTY(QColor windowColor READ windowColor NOTIFY themeColorsChanged)
    Q_PROPERTY(QColor windowTextColor READ windowTextColor NOTIFY themeColorsChanged)
    Q_PROPERTY(QColor baseColor READ baseColor NOTIFY themeColorsChanged)
    Q_PROPERTY(QColor alternateBaseColor READ alternateBaseColor NOTIFY themeColorsChanged)
    Q_PROPERTY(QColor textColor READ textColor NOTIFY themeColorsChanged)
    Q_PROPERTY(QColor buttonColor READ buttonColor NOTIFY themeColorsChanged)
    Q_PROPERTY(QColor buttonTextColor READ buttonTextColor NOTIFY themeColorsChanged)
    Q_PROPERTY(QColor highlightColor READ highlightColor NOTIFY themeColorsChanged)
    Q_PROPERTY(QColor highlightedTextColor READ highlightedTextColor NOTIFY themeColorsChanged)
    Q_PROPERTY(QColor linkColor READ linkColor NOTIFY themeColorsChanged)
    Q_PROPERTY(QColor fadeColor READ fadeColor NOTIFY themeColorsChanged)

    // Theme metadata properties
    Q_PROPERTY(QString currentThemeName READ currentThemeName NOTIFY currentThemeChanged)
    Q_PROPERTY(QString currentThemeTooltip READ currentThemeTooltip NOTIFY currentThemeChanged)
    Q_PROPERTY(double fadeAmount READ fadeAmount NOTIFY themeColorsChanged)
    Q_PROPERTY(bool hasStyleSheet READ hasStyleSheet NOTIFY currentThemeChanged)
    Q_PROPERTY(QString qtTheme READ qtTheme NOTIFY currentThemeChanged)

   public:
    explicit ThemeViewModel(QObject* parent = nullptr);

    QString currentTheme() const;

    QString currentIconTheme() const;

    QAbstractListModel* availableThemes() const;
    QAbstractListModel* availableIconThemes() const;

    // Theme color getters
    QColor windowColor() const;
    QColor windowTextColor() const;
    QColor baseColor() const;
    QColor alternateBaseColor() const;
    QColor textColor() const;
    QColor buttonColor() const;
    QColor buttonTextColor() const;
    QColor highlightColor() const;
    QColor highlightedTextColor() const;
    QColor linkColor() const;
    QColor fadeColor() const;

    // Theme metadata getters
    QString currentThemeName() const;
    QString currentThemeTooltip() const;
    double fadeAmount() const;
    bool hasStyleSheet() const;
    QString qtTheme() const;

    Q_INVOKABLE void applyTheme();
    Q_INVOKABLE void refreshThemes();
    Q_INVOKABLE void setCurrentTheme(const QString& themeId);
    Q_INVOKABLE void setCurrentIconTheme(const QString& themeId);

   signals:
    void currentThemeChanged();
    void currentIconThemeChanged();
    void themeColorsChanged();

   private:
    class ThemeListModel;
    class IconThemeListModel;

    ThemeListModel* m_themeListModel;
    IconThemeListModel* m_iconThemeListModel;
};
