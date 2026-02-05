// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qaccessiblequicktextinput_p.h"

QT_BEGIN_NAMESPACE

#if QT_CONFIG(accessibility)

QAccessibleQuickTextInput::QAccessibleQuickTextInput(QQuickTextInput *textEdit)
    : QAccessibleQuickItem(textEdit)
{
}

void QAccessibleQuickTextInput::removeSelection(int selectionIndex)
{
    if (selectionCount() == 1 && selectionIndex == 0) {
        const int cursorPos = textInput()->cursorPosition();
        textInput()->select(cursorPos, cursorPos);
    }
}

void QAccessibleQuickTextInput::setSelection(int selectionIndex, int startOffset, int endOffset)
{
    if (selectionIndex == 0)
        textInput()->select(startOffset, endOffset);
}

#endif // accessibility

QT_END_NAMESPACE
