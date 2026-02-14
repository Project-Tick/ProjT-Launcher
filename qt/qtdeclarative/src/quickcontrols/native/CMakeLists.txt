# Copyright (C) 2026 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

#####################################################################
## QuickControls2Native Module:
#####################################################################


if(ANDROID AND QT_FEATURE_quickcontrols2_material)
    set(native_import "QtQuick.Controls.Material")
    set(native_library Qt::QuickControls2MaterialPrivate)
    set(native_plugin qtquickcontrols2materialstyleplugin)
elseif(IOS AND QT_FEATURE_quickcontrols2_ios)
    set(native_import "QtQuick.Controls.iOS")
    set(native_library Qt::QuickControls2IOSPrivate)
    set(native_plugin qtquickcontrols2iosstyleplugin)
elseif(LINUX AND QT_FEATURE_quickcontrols2_fusion)
    set(native_import "QtQuick.Controls.Fusion")
    set(native_library Qt::QuickControls2FusionPrivate)
    set(native_plugin qtquickcontrols2fusionstyleplugin)
elseif(MACOS AND QT_FEATURE_quickcontrols2_macos)
    set(native_import "QtQuick.Controls.macOS")
    set(native_library Qt::QuickControls2Macos)
    set(native_plugin qtquickcontrols2macosstyleplugin)
elseif(WIN32 AND QT_FEATURE_quickcontrols2_windows)
    set(native_import "QtQuick.Controls.Windows")
    set(native_library Qt::QuickControls2Windows)
    set(native_plugin qtquickcontrols2windowsstyleplugin)
else()
    set(native_import "QtQuick.Controls.Basic")
    set(native_library Qt::QuickControls2BasicPrivate)
    set(native_plugin qtquickcontrols2basicstyleplugin)
endif()

qt_internal_add_qml_module(QuickControls2Native
    URI "QtQuick.Controls.Native"
    VERSION "${PROJECT_VERSION}"
    PLUGIN_TARGET qtquickcontrols2nativestyleplugin
    NO_PRIVATE_MODULE
    IMPORTS
        ${native_import}/auto
    LIBRARIES
        ${native_library}
)

_qt_internal_add_qml_static_plugin_dependency(qtquickcontrols2nativestyleplugin ${native_plugin})
