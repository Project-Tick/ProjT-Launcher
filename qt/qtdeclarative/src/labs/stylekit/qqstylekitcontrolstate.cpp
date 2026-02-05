// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qqstylekitcontrolstate_p.h"
#include "qqstylekitcontrol_p.h"

QT_BEGIN_NAMESPACE

QQStyleKitControlState::QQStyleKitControlState(QObject *parent)
    : QQStyleKitControlProperties(QQSK::PropertyGroup::Control, parent)
{
}

QQStyleKitControl *QQStyleKitControlState::control() const
{
    if (m_nestedState == QQSK::StateFlag::Normal) {
        Q_ASSERT(qobject_cast<const QQStyleKitControl *>(this));
        auto *self = const_cast<QQStyleKitControlState *>(this);
        return static_cast<QQStyleKitControl *>(self);
    }
    Q_ASSERT(qobject_cast<const QQStyleKitControl *>(parent()));
    return static_cast<QQStyleKitControl *>(parent());
}

QQStyleKitControlState *QQStyleKitControlState::lazyCreateState(QQSK::StateFlag state) const
{
    if (m_nestedStateObjects.contains(state))
        return m_nestedStateObjects.value(state);

    QQStyleKitControlState *stateObj = new QQStyleKitControlState(control());
    stateObj->m_nestedState = m_nestedState;
    stateObj->m_nestedState.setFlag(state);
    stateObj->m_nestedState.setFlag(QQSK::StateFlag::Normal, false);

    auto *self = const_cast<QQStyleKitControlState *>(this);
    self->m_nestedStateObjects.insert(state, stateObj);

    return stateObj;
}

QQStyleKitControlState *QQStyleKitControlState::pressed() const
{
    return lazyCreateState(QQSK::StateFlag::Pressed);
}

QQStyleKitControlState *QQStyleKitControlState::hovered() const
{
    return lazyCreateState(QQSK::StateFlag::Hovered);
}

QQStyleKitControlState *QQStyleKitControlState::highlighted() const
{
    return lazyCreateState(QQSK::StateFlag::Highlighted);
}

QQStyleKitControlState *QQStyleKitControlState::focused() const
{
    return lazyCreateState(QQSK::StateFlag::Focused);
}

QQStyleKitControlState *QQStyleKitControlState::checked() const
{
    return lazyCreateState(QQSK::StateFlag::Checked);
}

QQStyleKitControlState *QQStyleKitControlState::vertical() const
{
    return lazyCreateState(QQSK::StateFlag::Vertical);
}

QQStyleKitControlState *QQStyleKitControlState::disabled() const
{
    return lazyCreateState(QQSK::StateFlag::Disabled);
}

QT_END_NAMESPACE

#include "moc_qqstylekitcontrolstate_p.cpp"
