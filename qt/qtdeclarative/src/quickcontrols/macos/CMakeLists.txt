# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

#####################################################################
## QuickControls2Macos Module:
#####################################################################

add_subdirectory(impl)

set(qml_files
    "BusyIndicator.qml"
    "Button.qml"
    "CheckBox.qml"
    "CheckDelegate.qml"
    "ComboBox.qml"
    "DelayButton.qml"
    "Dial.qml"
    "Dialog.qml"
    "DialogButtonBox.qml"
    "DoubleSpinBox.qml"
    "Frame.qml"
    "GroupBox.qml"
    "ItemDelegate.qml"
    "Menu.qml"
    "MenuBar.qml"
    "MenuBarItem.qml"
    "MenuItem.qml"
    "MenuSeparator.qml"
    "ProgressBar.qml"
    "RadioButton.qml"
    "RadioDelegate.qml"
    "RangeSlider.qml"
    "ScrollBar.qml"
    "ScrollIndicator.qml"
    "ScrollView.qml"
    "SearchField.qml"
    "SelectionRectangle.qml"
    "Slider.qml"
    "SpinBox.qml"
    "Switch.qml"
    "SwitchDelegate.qml"
    "TextArea.qml"
    "TextField.qml"
)

if (QT_FEATURE_quick_treeview)
    list(APPEND qml_files
        "TreeViewDelegate.qml"
    )
endif()

set_source_files_properties(SearchField.qml PROPERTIES
    QT_QML_SOURCE_VERSIONS "6.10"
)

qt_internal_add_qml_module(QuickControls2Macos
    URI "QtQuick.Controls.macOS"
    VERSION "${PROJECT_VERSION}"
    CLASS_NAME QtQuickControls2MacOSStylePlugin
    IMPORTS
        QtQuick.Controls.Fusion/auto
    PAST_MAJOR_VERSIONS 2
    PLUGIN_TARGET qtquickcontrols2macosstyleplugin
    NO_PLUGIN_OPTIONAL
    NO_GENERATE_PLUGIN_SOURCE
    NO_PRIVATE_MODULE
    QML_FILES
        ${qml_files}
    RESOURCES
        images/busyindicator-dark.webp
        images/busyindicator-dark@2x.webp
        images/busyindicator-dark@3x.webp
        images/busyindicator-light.webp
        images/busyindicator-light@2x.webp
        images/busyindicator-light@3x.webp
        images/checkmark.png
        images/checkmark@2x.png
        images/checkmark@3x.png
        images/menuarrow.png
        images/menuarrow@2x.png
        images/menuarrow@3x.png
    DEFINES
        QT_NO_CAST_FROM_ASCII
        QT_NO_CAST_TO_ASCII
    LIBRARIES
        Qt::CorePrivate
        Qt::GuiPrivate
        Qt::QmlPrivate
        Qt::QuickControls2MacOSStyleImpl
        Qt::QuickPrivate
        Qt::QuickControls2FusionPrivate
        Qt::QuickNativeStylePrivate
)

target_sources(qtquickcontrols2macosstyleplugin
    PRIVATE
        qtquickcontrols2macosstyleplugin.cpp
)

qt_internal_extend_target(qtquickcontrols2macosstyleplugin
    LIBRARIES
        Qt::QuickControls2Private
        Qt::QuickTemplates2Private
)

# Native style is a dependency of the macOS style.
_qt_internal_add_qml_static_plugin_dependency(qtquickcontrols2macosstyleplugin
                                              qtquicknativestyleplugin)

# Fusion style is the required fallback style.
_qt_internal_add_qml_static_plugin_dependency(qtquickcontrols2macosstyleplugin
                                              qtquickcontrols2fusionstyleplugin)
