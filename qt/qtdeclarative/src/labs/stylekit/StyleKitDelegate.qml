// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default

import QtQuick
import QtQuick.Effects
import QtQuick.Controls.impl
import Qt.labs.StyleKit
import Qt.labs.StyleKit.impl

/* This delegate takes care of loading the individual elements that are needed
    in order to draw it according to its configuration in the style.
    Since some delegates have a gradient, others an image, and some perhaps
    both, we use dynamic loading to reduce memory usage. We could use Loaders
    for this, but from testing, using JS appears visually more performant.
    TODO: move most of this code to c++? Collapse all elements into a single
    scene-graph node? Redraw everything from a single update, rather than using
    property bindings?
*/
Item {
    id: root

    // If an implicit size has been set on the delegate in the style, it
    // wins. Otherwise we use the implicit size of the image
    implicitWidth: __imageInstance ? __imageInstance.implicitWidth : 0
    implicitHeight: __imageInstance ? __imageInstance.implicitHeight : 0
    width: parent.width
    height: parent.height
    opacity: delegateProperties.opacity

    required property QtObject control
    required property StyleKitDelegateProperties delegateProperties

    property Item __colorInstance: null
    property Item __gradientInstance: null
    property Item __imageInstance: null

    onDelegatePropertiesChanged: {
        instantiateColorDelegate()
        instantiateGradientDelegate()
        instantiateImageDelegate()
    }

    Connections {
        target: delegateProperties
        function onColorChanged() { instantiateColorDelegate() }
        function onGradientChanged() { instantiateGradientDelegate() }
    }

    Connections {
        target: delegateProperties.image
        function onSourceChanged() { instantiateImageDelegate() }
        function onColorChanged() { instantiateImageDelegate() }
    }

    Component {
        id: colorDelegate
        Rectangle {
            required property StyleKitDelegateProperties delegateProperties
            width: parent.width
            height: parent.height
            topLeftRadius: delegateProperties.topLeftRadius
            topRightRadius: delegateProperties.topRightRadius
            bottomLeftRadius: delegateProperties.bottomLeftRadius
            bottomRightRadius: delegateProperties.bottomRightRadius
            color: delegateProperties.color
            border.width: delegateProperties.border.width
            border.color: delegateProperties.border.color
        }
    }

    Component {
        id: gradientDelegate
        Rectangle {
            required property StyleKitDelegateProperties delegateProperties
            width: parent.width
            height: parent.height
            topLeftRadius: delegateProperties.topLeftRadius
            topRightRadius: delegateProperties.topRightRadius
            bottomLeftRadius: delegateProperties.bottomLeftRadius
            bottomRightRadius: delegateProperties.bottomRightRadius
            border.width: delegateProperties.border.width
            border.color: delegateProperties.border.color
            gradient: delegateProperties.gradient
            visible: delegateProperties.gradient !== null
        }
    }

    Component {
        id: imageDelegate
        ColorImage {
            required property StyleKitDelegateProperties delegateProperties
            width: parent.width
            height: parent.height
            color: delegateProperties.image.color
            source: delegateProperties.image.source
            fillMode: delegateProperties.image.fillMode
            visible: status === Image.Ready
        }
    }

    function instantiateColorDelegate() {
        if (__colorInstance)
            return

        // Delay instantiating element until needed
        if (delegateProperties.color.a === 0 &&
                (delegateProperties.border.color.a === 0
                 || delegateProperties.border.width === 0))
            return

        /* Note: we adjust z to ensure the elements are stacked correct, no matter the
         * order in which they are instantiated. And we use negative z values to ensure
         * that any children (with a default z === 0) of a "subclass" of the StyleKitDelegate
         * ends up on top of this again. */
        let prevInstance = __colorInstance
        __colorInstance = colorDelegate.createObject(root, { z: -3, delegateProperties: root.delegateProperties })
        if (prevInstance)
            prevInstance.destroy()
    }


    function instantiateGradientDelegate() {
        if (__gradientInstance)
            return

        // Delay instantiating element until needed
        if (delegateProperties.gradient === null)
            return

        let prevInstance = __gradientInstance
        __gradientInstance = gradientDelegate.createObject(root, { z: -2, delegateProperties: root.delegateProperties })
        if (prevInstance)
            prevInstance.destroy()
    }

    function instantiateImageDelegate() {
        if (__imageInstance)
            return

        // Delay instantiating element until needed
        if (delegateProperties.image.source === "" || delegateProperties.image.color.a === 0)
            return

        let prevInstance = __imageInstance
        __imageInstance = imageDelegate.createObject(root, { z: -1, delegateProperties: root.delegateProperties })
        if (prevInstance)
            prevInstance.destroy()
    }

}
