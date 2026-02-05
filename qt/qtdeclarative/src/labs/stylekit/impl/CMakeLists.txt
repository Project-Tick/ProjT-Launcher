# Copyright (C) 2025 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

#####################################################################
## StyleKitImpl Module:
#####################################################################

set_source_files_properties(DelegateSingleton.qml
    PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

qt_internal_add_qml_module(QtQuickStyleKitImpl
    URI Qt.labs.StyleKit.impl
    DEPENDENCIES
        QtQuick/auto
    SOURCES
        qqstylekitlayout_p.h qqstylekitlayout.cpp
    QML_FILES
        BackgroundAndIndicatorDelegate.qml
        BackgroundDelegate.qml
        DelegateContainer.qml
        DelegateSingleton.qml
        FallbackStyle.qml
        HandleDelegate.qml
        IndicatorDelegate.qml
        Shadow.qml
    LIBRARIES
        Qt6::GuiPrivate
        Qt6::Quick
        Qt6::QuickPrivate
        Qt6::QuickTemplates2
        Qt6::QuickTemplates2Private
)
