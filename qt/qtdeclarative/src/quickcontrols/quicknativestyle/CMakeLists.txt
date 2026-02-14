# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

#####################################################################
## QuickNativeStyle Module:
#####################################################################

qt_policy(SET QTP0004 NEW)

set(qml_files
    "controls/DefaultButton.qml"
    "controls/DefaultCheckBox.qml"
    "controls/DefaultComboBox.qml"
    "controls/DefaultDial.qml"
    "controls/DefaultDoubleSpinBox.qml"
    "controls/DefaultFrame.qml"
    "controls/DefaultGroupBox.qml"
    "controls/DefaultItemDelegate.qml"
    "controls/DefaultItemDelegateIconLabel.qml"
    "controls/DefaultProgressBar.qml"
    "controls/DefaultRadioButton.qml"
    "controls/DefaultRadioDelegate.qml"
    "controls/DefaultScrollBar.qml"
    "controls/DefaultSearchField.qml"
    "controls/DefaultSlider.qml"
    "controls/DefaultSpinBox.qml"
    "controls/DefaultTextArea.qml"
    "controls/DefaultTextField.qml"
)

if(MACOS)
    list(APPEND qml_files "util/MacFocusFrame.qml")
elseif(WIN32)
    list(APPEND qml_files "util/WindowsFocusFrame.qml")
endif()

if(QT_FEATURE_quick_treeview)
    list(APPEND qml_files "controls/DefaultTreeViewDelegate.qml")
endif()

set_source_files_properties(SearchField.qml PROPERTIES
    QT_QML_SOURCE_VERSIONS "6.10"
)

qt_internal_add_qml_module(QuickNativeStyle
    URI "QtQuick.NativeStyle"
    VERSION "${PROJECT_VERSION}"
    CLASS_NAME QtQuickControls2NativeStylePlugin
    DEPENDENCIES
        QtQuick.Templates/auto
        QtQuick.Layouts/auto
        QtQuick/auto
    PAST_MAJOR_VERSIONS 2
    PLUGIN_TARGET qtquicknativestyleplugin
    NO_PLUGIN_OPTIONAL
    NO_GENERATE_PLUGIN_SOURCE
    SOURCES
        items/qquickstyleitem.cpp items/qquickstyleitem_p.h
        items/qquickstyleitembutton.cpp items/qquickstyleitembutton_p.h
        items/qquickstyleitemcheckbox.cpp items/qquickstyleitemcheckbox_p.h
        items/qquickstyleitemdelaybutton.cpp items/qquickstyleitemdelaybutton_p.h
        items/qquickstyleitemdoublespinbox.cpp items/qquickstyleitemdoublespinbox_p.h
        items/qquickstyleitemcheckdelegate.cpp items/qquickstyleitemcheckdelegate_p.h
        items/qquickstyleitemdial.cpp items/qquickstyleitemdial_p.h
        items/qquickstyleitemframe.cpp items/qquickstyleitemframe_p.h
        items/qquickstyleitemgroupbox.cpp items/qquickstyleitemgroupbox_p.h
        items/qquickstyleitemprogressbar.cpp items/qquickstyleitemprogressbar_p.h
        items/qquickstyleitemradiobutton.cpp items/qquickstyleitemradiobutton_p.h
        items/qquickstyleitemradiodelegate.cpp items/qquickstyleitemradiodelegate_p.h
        items/qquickstyleitemscrollbar.cpp items/qquickstyleitemscrollbar_p.h
        items/qquickstyleitemslider.cpp items/qquickstyleitemslider_p.h
        items/qquickstyleitemspinbox.cpp items/qquickstyleitemspinbox_p.h
        items/qquickstyleitemtextfield.cpp items/qquickstyleitemtextfield_p.h
        qstyle/qqc2qcommonstyle.cpp qstyle/qqc2qcommonstyle_p.h qstyle/qqc2qcommonstyle_p_p.h
        qstyle/qqc2qcommonstylepixmaps_p.h
        qstyle/qqc2qdrawutil.cpp qstyle/qqc2qdrawutil_p.h
        qstyle/qqc2qnativestyle.cpp qstyle/qqc2qnativestyle_p.h
        qstyle/qqc2qstyle.cpp qstyle/qqc2qstyle_p.h qstyle/qqc2qstyle_p_p.h
        qstyle/qqc2qstylehelper.cpp qstyle/qqc2qstylehelper_p.h
        qstyle/qqc2qstyleoption.cpp qstyle/qqc2qstyleoption_p.h
        util/qquickfocusframe.cpp util/qquickfocusframe_p.h
    QML_FILES
        ${qml_files}
    DEFINES
        QT_NO_CAST_FROM_ASCII
        QT_NO_CAST_TO_ASCII
    INCLUDE_DIRECTORIES
        items
        qstyle
        util
    LIBRARIES
        Qt::CorePrivate
        Qt::GuiPrivate
        Qt::QmlPrivate
        Qt::QuickPrivate
        Qt::QuickTemplates2Private
    PUBLIC_LIBRARIES
        Qt::Core
        Qt::Gui
        Qt::Qml
        Qt::Quick
        Qt::QuickTemplates2
    PRIVATE_MODULE_INTERFACE
        Qt::QuickPrivate
        Qt::QuickTemplates2Private
)

qt_internal_extend_target(QuickNativeStyle CONDITION QT_FEATURE_quick_treeview
    SOURCES
        items/qquickstyleitemtreeindicator.cpp items/qquickstyleitemtreeindicator_p.h
)

qt_internal_extend_target(QuickNativeStyle CONDITION QT_FEATURE_qml_delegate_model
    SOURCES
        items/qquickstyleitemcombobox.cpp items/qquickstyleitemcombobox_p.h
        items/qquickstyleitemsearchfield.cpp items/qquickstyleitemsearchfield_p.h
)

qt_internal_extend_target(QuickNativeStyle CONDITION MACOS
    SOURCES
        items/qquickstyleitemscrollviewcorner.cpp items/qquickstyleitemscrollviewcorner_p.h
        qstyle/mac/qquickmacstyle_mac.mm qstyle/mac/qquickmacstyle_mac_p.h
        qstyle/mac/qquickmacstyle_mac_p_p.h
        util/qquickmacfocusframe.mm util/qquickmacfocusframe_p.h
        util/qquickstyleconstants.mm util/qquickstyleconstants_p.h
    INCLUDE_DIRECTORIES
        qstyle/mac
    LIBRARIES
        ${FWAppKit}
)

qt_internal_extend_target(QuickNativeStyle CONDITION WIN32
    SOURCES
        qstyle/windows/qquickwindowsstyle.cpp qstyle/windows/qquickwindowsstyle_p.h
        qstyle/windows/qquickwindowsstyle_p_p.h
        qstyle/windows/qquickwindowsxpstyle.cpp
        qstyle/windows/qquickwindowsxpstyle_p.h
        qstyle/windows/qquickwindowsxpstyle_p_p.h
        util/qquickwindowsfocusframe.cpp util/qquickwindowsfocusframe_p.h
    INCLUDE_DIRECTORIES
        qstyle/windows
    LIBRARIES
        gdi32
        user32
        uxtheme
)

target_sources(qtquicknativestyleplugin
    PRIVATE
        qtquickcontrols2nativestyleplugin.cpp
)

qt_internal_extend_target(qtquicknativestyleplugin
    LIBRARIES
        Qt::QuickControls2Private
        Qt::QuickNativeStylePrivate
)
