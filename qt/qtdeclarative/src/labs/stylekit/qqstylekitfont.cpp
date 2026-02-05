// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qqstylekitfont_p.h"

QT_BEGIN_NAMESPACE

QQStyleKitFont::QQStyleKitFont(QObject *parent)
    : QObject(parent)
{
}

#define DEFINE_FONT_GETTER(scopeName, scopeEnum) \
    QFont QQStyleKitFont::scopeName() const \
    { \
        return fontForScope(QQuickTheme::scopeEnum); \
    }

DEFINE_FONT_GETTER(system, System)
DEFINE_FONT_GETTER(button, Button)
DEFINE_FONT_GETTER(checkBox, CheckBox)
DEFINE_FONT_GETTER(comboBox, ComboBox)
DEFINE_FONT_GETTER(groupBox, GroupBox)
DEFINE_FONT_GETTER(itemView, ItemView)
DEFINE_FONT_GETTER(label, Label)
DEFINE_FONT_GETTER(listView, ListView)
DEFINE_FONT_GETTER(menu, Menu)
DEFINE_FONT_GETTER(menuBar, MenuBar)
DEFINE_FONT_GETTER(radioButton, RadioButton)
DEFINE_FONT_GETTER(spinBox, SpinBox)
DEFINE_FONT_GETTER(switchControl, Switch)
DEFINE_FONT_GETTER(tabBar, TabBar)
DEFINE_FONT_GETTER(textArea, TextArea)
DEFINE_FONT_GETTER(textField, TextField)
DEFINE_FONT_GETTER(toolBar, ToolBar)
DEFINE_FONT_GETTER(toolTip, ToolTip)
DEFINE_FONT_GETTER(tumbler, Tumbler)

#define DEFINE_FONT_SETTER(scopeName, scopeEnum, signal) \
    void QQStyleKitFont::set##scopeName(const QFont &font) \
    { \
        setFontForScope(QQuickTheme::scopeEnum, font, &QQStyleKitFont::signal); \
    }

DEFINE_FONT_SETTER(System, System, systemChanged)
DEFINE_FONT_SETTER(Button, Button, buttonChanged)
DEFINE_FONT_SETTER(CheckBox, CheckBox, checkBoxChanged)
DEFINE_FONT_SETTER(ComboBox, ComboBox, comboBoxChanged)
DEFINE_FONT_SETTER(GroupBox, GroupBox, groupBoxChanged)
DEFINE_FONT_SETTER(ItemView, ItemView, itemViewChanged)
DEFINE_FONT_SETTER(Label, Label, labelChanged)
DEFINE_FONT_SETTER(ListView, ListView, listViewChanged)
DEFINE_FONT_SETTER(Menu, Menu, menuChanged)
DEFINE_FONT_SETTER(MenuBar, MenuBar, menuBarChanged)
DEFINE_FONT_SETTER(RadioButton, RadioButton, radioButtonChanged)
DEFINE_FONT_SETTER(SpinBox, SpinBox, spinBoxChanged)
DEFINE_FONT_SETTER(SwitchControl, Switch, switchControlChanged)
DEFINE_FONT_SETTER(TabBar, TabBar, tabBarChanged)
DEFINE_FONT_SETTER(TextArea, TextArea, textAreaChanged)
DEFINE_FONT_SETTER(TextField, TextField, textFieldChanged)
DEFINE_FONT_SETTER(ToolBar, ToolBar, toolBarChanged)
DEFINE_FONT_SETTER(ToolTip, ToolTip, toolTipChanged)
DEFINE_FONT_SETTER(Tumbler, Tumbler, tumblerChanged)

void QQStyleKitFont::setFontForScope(QQuickTheme::Scope scope, const QFont &font, void (QQStyleKitFont::*signal)())
{
    const int index = int(scope);
    if (isSet(scope) && m_local[index] == font)
        return;

    QFont local = font;
    // TODO: Figure out resolve mask to set here

    m_local[index] = font;
    markSet(scope);

    m_effectiveDirty = true;

    emit (this->*signal)();
}

// The fallback font is used to resolve unset fonts
// The theme fonts fallback to the style fonts and
// style fonts fallback to the fallback style fonts
QQStyleKitFont *QQStyleKitFont::fallbackFont() const
{
    return m_fallback;
}

void QQStyleKitFont::setFallbackFont(QQStyleKitFont *fallback)
{
    if (m_fallback == fallback)
        return;

    if (m_fallback)
        disconnect(m_fallback, nullptr, this, nullptr);

    m_fallback = fallback;

    markEffectiveDirty();
    ensureEffectiveUpToDate();

    if (m_fallback) {
        auto makeHandler = [this](void (QQStyleKitFont::*signal)()) {
            return [this, signal] {
                markEffectiveDirty();
                emit (this->*signal)();
            };
        };
        connect(m_fallback, &QQStyleKitFont::systemChanged, this,
            makeHandler(&QQStyleKitFont::systemChanged));
        connect(m_fallback, &QQStyleKitFont::buttonChanged, this,
            makeHandler(&QQStyleKitFont::buttonChanged));
        connect(m_fallback, &QQStyleKitFont::checkBoxChanged, this,
            makeHandler(&QQStyleKitFont::checkBoxChanged));
        connect(m_fallback, &QQStyleKitFont::comboBoxChanged, this,
            makeHandler(&QQStyleKitFont::comboBoxChanged));
        connect(m_fallback, &QQStyleKitFont::groupBoxChanged, this,
            makeHandler(&QQStyleKitFont::groupBoxChanged));
        connect(m_fallback, &QQStyleKitFont::itemViewChanged, this,
            makeHandler(&QQStyleKitFont::itemViewChanged));
        connect(m_fallback, &QQStyleKitFont::labelChanged, this,
            makeHandler(&QQStyleKitFont::labelChanged));
        connect(m_fallback, &QQStyleKitFont::listViewChanged, this,
            makeHandler(&QQStyleKitFont::listViewChanged));
        connect(m_fallback, &QQStyleKitFont::menuChanged, this,
            makeHandler(&QQStyleKitFont::menuChanged));
        connect(m_fallback, &QQStyleKitFont::menuBarChanged, this,
            makeHandler(&QQStyleKitFont::menuBarChanged));
        connect(m_fallback, &QQStyleKitFont::radioButtonChanged, this,
            makeHandler(&QQStyleKitFont::radioButtonChanged));
        connect(m_fallback, &QQStyleKitFont::spinBoxChanged, this,
            makeHandler(&QQStyleKitFont::spinBoxChanged));
        connect(m_fallback, &QQStyleKitFont::switchControlChanged, this,
            makeHandler(&QQStyleKitFont::switchControlChanged));
        connect(m_fallback, &QQStyleKitFont::tabBarChanged, this,
            makeHandler(&QQStyleKitFont::tabBarChanged));
        connect(m_fallback, &QQStyleKitFont::textAreaChanged, this,
            makeHandler(&QQStyleKitFont::textAreaChanged));
        connect(m_fallback, &QQStyleKitFont::textFieldChanged, this,
            makeHandler(&QQStyleKitFont::textFieldChanged));
        connect(m_fallback, &QQStyleKitFont::toolBarChanged, this,
            makeHandler(&QQStyleKitFont::toolBarChanged));
        connect(m_fallback, &QQStyleKitFont::toolTipChanged, this,
            makeHandler(&QQStyleKitFont::toolTipChanged));
        connect(m_fallback, &QQStyleKitFont::tumblerChanged, this,
            makeHandler(&QQStyleKitFont::tumblerChanged));
    }
    emit fallbackFontChanged();
}

QFont QQStyleKitFont::fontForScope(QQuickTheme::Scope scope) const
{
    ensureEffectiveUpToDate();
    return m_effective[int(scope)];
}

void QQStyleKitFont::ensureEffectiveUpToDate() const
{
    if (!m_effectiveDirty)
        return;

    const int sysIdx = int(QQuickTheme::System);

    {
        const QFont localSys = isSet(QQuickTheme::System) ? m_local[sysIdx] : QFont();
        const QFont fbSys = m_fallback ? m_fallback->fontForScope(QQuickTheme::System) : QFont();
        // TODO: Resolve mask?
        m_effective[sysIdx] = localSys.resolve(fbSys);

    }

    const QFont systemEff = m_effective[sysIdx];
    const QFont fallbackSystem = m_fallback ? m_fallback->fontForScope(QQuickTheme::System) : QFont();

    // Scopes: localScope > localSystem > fallbackScope > fallbackSystem
    for (int i = 0; i < NScopes; ++i) {
        if (i == sysIdx)
            continue;

        const QQuickTheme::Scope scope = static_cast<QQuickTheme::Scope>(i);
        const QFont localRole = isSet(scope) ? m_local[i] : QFont();
        const QFont fallbackRole = m_fallback ? m_fallback->fontForScope(scope) : QFont();

        QFont fallbackLayer = fallbackRole.resolve(fallbackSystem);
        QFont base = systemEff.resolve(fallbackLayer);
        // TODO: Resolve mask?
        m_effective[i] = localRole.resolve(base);
    }

    m_effectiveDirty = false;
}

QT_END_NAMESPACE
#include "moc_qqstylekitfont_p.cpp"
