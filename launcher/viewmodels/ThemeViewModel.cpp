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

#include "ThemeViewModel.h"

#include "Application.h"
#include "ui/themes/ITheme.h"
#include "ui/themes/IconTheme.h"
#include "ui/themes/ThemeManager.h"

// Theme List Model
class ThemeViewModel::ThemeListModel : public QAbstractListModel {
   public:
    enum Role { IdRole = Qt::UserRole + 1, NameRole, TooltipRole };

    explicit ThemeListModel(QObject* parent = nullptr) : QAbstractListModel(parent) { refresh(); }

    int rowCount(const QModelIndex& parent = QModelIndex()) const override
    {
        Q_UNUSED(parent);
        return m_themes.count();
    }

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override
    {
        if (!index.isValid() || index.row() >= m_themes.count())
            return QVariant();

        auto* theme = m_themes[index.row()];
        switch (role) {
        case IdRole:
            return theme->id();
        case NameRole:
        case Qt::DisplayRole:
            return theme->name();
        case TooltipRole:
            return theme->tooltip();
        default:
            return QVariant();
        }
    }

    QHash<int, QByteArray> roleNames() const override
    {
        return { { IdRole, "themeId" }, { NameRole, "name" }, { TooltipRole, "tooltip" } };
    }

    void refresh()
    {
        beginResetModel();
        m_themes = APPLICATION->themeManager()->getValidApplicationThemes();
        endResetModel();
    }

   private:
    QList<ITheme*> m_themes;
};

// Icon Theme List Model
class ThemeViewModel::IconThemeListModel : public QAbstractListModel {
   public:
    enum Role { IdRole = Qt::UserRole + 1, NameRole, PathRole };

    explicit IconThemeListModel(QObject* parent = nullptr) : QAbstractListModel(parent) { refresh(); }

    int rowCount(const QModelIndex& parent = QModelIndex()) const override
    {
        Q_UNUSED(parent);
        return m_iconThemes.count();
    }

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override
    {
        if (!index.isValid() || index.row() >= m_iconThemes.count())
            return QVariant();

        auto* iconTheme = m_iconThemes[index.row()];
        switch (role) {
        case IdRole:
            return iconTheme->id();
        case NameRole:
        case Qt::DisplayRole:
            return iconTheme->name();
        case PathRole:
            return iconTheme->path();
        default:
            return QVariant();
        }
    }

    QHash<int, QByteArray> roleNames() const override { return { { IdRole, "themeId" }, { NameRole, "name" }, { PathRole, "path" } }; }

    void refresh()
    {
        beginResetModel();
        m_iconThemes = APPLICATION->themeManager()->getValidIconThemes();
        endResetModel();
    }

   private:
    QList<IconTheme*> m_iconThemes;
};

// ThemeViewModel Implementation
ThemeViewModel::ThemeViewModel(QObject* parent)
    : QObject(parent), m_themeListModel(new ThemeListModel(this)), m_iconThemeListModel(new IconThemeListModel(this))
{}

QString ThemeViewModel::currentTheme() const
{
    return APPLICATION->settings()->get("ApplicationTheme").toString();
}

void ThemeViewModel::setCurrentTheme(const QString& themeId)
{
    if (currentTheme() == themeId)
        return;

    APPLICATION->settings()->set("ApplicationTheme", themeId);
    emit currentThemeChanged();

    // Apply the theme immediately
    APPLICATION->themeManager()->applyCurrentlySelectedTheme();
    emit themeColorsChanged();
}

QString ThemeViewModel::currentIconTheme() const
{
    return APPLICATION->settings()->get("IconTheme").toString();
}

void ThemeViewModel::setCurrentIconTheme(const QString& themeId)
{
    if (currentIconTheme() == themeId)
        return;

    APPLICATION->settings()->set("IconTheme", themeId);
    emit currentIconThemeChanged();

    // Apply the icon theme immediately
    APPLICATION->themeManager()->setIconTheme(themeId);
    APPLICATION->themeManager()->applyCurrentlySelectedTheme();
    emit themeColorsChanged();
}

QAbstractListModel* ThemeViewModel::availableThemes() const
{
    return m_themeListModel;
}

QAbstractListModel* ThemeViewModel::availableIconThemes() const
{
    return m_iconThemeListModel;
}

void ThemeViewModel::applyTheme()
{
    APPLICATION->themeManager()->applyCurrentlySelectedTheme();
    emit themeColorsChanged();
}

void ThemeViewModel::refreshThemes()
{
    APPLICATION->themeManager()->refresh();
    m_themeListModel->refresh();
    m_iconThemeListModel->refresh();
    emit themeColorsChanged();
}

// Theme color getters
QColor ThemeViewModel::windowColor() const
{
    return qApp->palette().color(QPalette::Window);
}

QColor ThemeViewModel::windowTextColor() const
{
    return qApp->palette().color(QPalette::WindowText);
}

QColor ThemeViewModel::baseColor() const
{
    return qApp->palette().color(QPalette::Base);
}

QColor ThemeViewModel::alternateBaseColor() const
{
    return qApp->palette().color(QPalette::AlternateBase);
}

QColor ThemeViewModel::textColor() const
{
    return qApp->palette().color(QPalette::Text);
}

QColor ThemeViewModel::buttonColor() const
{
    return qApp->palette().color(QPalette::Button);
}

QColor ThemeViewModel::buttonTextColor() const
{
    return qApp->palette().color(QPalette::ButtonText);
}

QColor ThemeViewModel::highlightColor() const
{
    return qApp->palette().color(QPalette::Highlight);
}

QColor ThemeViewModel::highlightedTextColor() const
{
    return qApp->palette().color(QPalette::HighlightedText);
}

QColor ThemeViewModel::linkColor() const
{
    return qApp->palette().color(QPalette::Link);
}

QColor ThemeViewModel::fadeColor() const
{
    auto* themeManager = APPLICATION->themeManager();
    auto themes = themeManager->getValidApplicationThemes();
    QString currentThemeId = currentTheme();

    for (auto* theme : themes) {
        if (theme && theme->id() == currentThemeId) {
            return theme->fadeColor();
        }
    }

    // Default fade color
    return QColor(0, 0, 0);
}

// Theme metadata getters
QString ThemeViewModel::currentThemeName() const
{
    auto* themeManager = APPLICATION->themeManager();
    auto themes = themeManager->getValidApplicationThemes();
    QString currentThemeId = currentTheme();

    for (auto* theme : themes) {
        if (theme && theme->id() == currentThemeId) {
            return theme->name();
        }
    }

    return QStringLiteral("Unknown");
}

QString ThemeViewModel::currentThemeTooltip() const
{
    auto* themeManager = APPLICATION->themeManager();
    auto themes = themeManager->getValidApplicationThemes();
    QString currentThemeId = currentTheme();

    for (auto* theme : themes) {
        if (theme && theme->id() == currentThemeId) {
            return theme->tooltip();
        }
    }

    return QString();
}

double ThemeViewModel::fadeAmount() const
{
    auto* themeManager = APPLICATION->themeManager();
    auto themes = themeManager->getValidApplicationThemes();
    QString currentThemeId = currentTheme();

    for (auto* theme : themes) {
        if (theme && theme->id() == currentThemeId) {
            return theme->fadeAmount();
        }
    }

    return 0.5;
}

bool ThemeViewModel::hasStyleSheet() const
{
    auto* themeManager = APPLICATION->themeManager();
    auto themes = themeManager->getValidApplicationThemes();
    QString currentThemeId = currentTheme();

    for (auto* theme : themes) {
        if (theme && theme->id() == currentThemeId) {
            return theme->hasStyleSheet();
        }
    }

    return false;
}

QString ThemeViewModel::qtTheme() const
{
    auto* themeManager = APPLICATION->themeManager();
    auto themes = themeManager->getValidApplicationThemes();
    QString currentThemeId = currentTheme();

    for (auto* theme : themes) {
        if (theme && theme->id() == currentThemeId) {
            return theme->qtTheme();
        }
    }

    return QString();
}
