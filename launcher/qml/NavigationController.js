// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
 *
 *  This file is part of ProjT Launcher and is licensed under
 *  the GNU General Public License version 3 or later.
 *
 *  If this file includes work from previous open-source projects,
 *  their original copyright and license notices are preserved below.
 */
.pragma library

var stackView = null
var navModel = null
var busyCheck = function() { return false }
var onRouteChanged = function() {}
var ComponentNull = 0
var ComponentReady = 1
var ComponentLoading = 2
var ComponentError = 3

function configure(stack, model, busyFn, routeChangedCb) {
    stackView = stack
    navModel = model
    busyCheck = busyFn || function() { return false }
    onRouteChanged = routeChangedCb || function() {}
}

function sourceFor(route) {
    if (!navModel || navModel.count === 0) {
        return ""
    }
    for (var i = 0; i < navModel.count; ++i) {
        var entry = navModel.get(i)
        if (entry.route === route) {
            var s = entry.source || ""
            if (s.length === 0) {
                return ""
            }
            if (s.indexOf("qrc:/") === 0 || s.indexOf("file:/") === 0) {
                return s
            }
            return s
        }
    }
    return ""
}

function go(route) {
    if (!stackView) {
        return false
    }
    if (route === currentRoute() && stackView.depth > 0) {
        return true
    }
    if (busyCheck && busyCheck()) {
        return false
    }
    var src = sourceFor(route)
    if (!src) {
        return false
    }
    var candidates = []
    candidates.push(Qt.resolvedUrl(src))
    var baseName = src
    var lastSlash = src.lastIndexOf("/")
    if (lastSlash >= 0) {
        baseName = src.substring(lastSlash + 1)
    }
    candidates.push("qrc:/qml/" + baseName)

    for (var idx = 0; idx < candidates.length; ++idx) {
        var target = candidates[idx]
        var component = Qt.createComponent(target)
        if (component.status === ComponentLoading) {
            component.statusChanged.connect(function(status) {
                if (status === ComponentReady) {
                    var pageLoaded = component.createObject(stackView)
                    if (pageLoaded) {
                        stackView.clear()
                        stackView.push(pageLoaded)
                        onRouteChanged(route)
                    }
                    component.destroy()
                } else if (status === ComponentError) {
                    component.destroy()
                }
            })
            return true
        }
        if (component.status === ComponentReady) {
            var page = component.createObject(stackView)
            if (page) {
                stackView.clear()
                stackView.push(page)
                onRouteChanged(route)
                component.destroy()
                return true
            }
            component.destroy()
        } else {
            component.destroy()
        }
    }
    console.warn("Navigation: failed to load route", route, "after trying", candidates)
    return false
}

function goToInstances() { return go("instances") }
function goToNews() { return go("news") }
function goToSettings() { return go("settings") }
function goToAbout() { return go("about") }

function currentRoute() {
    if (!stackView || !stackView.currentItem) {
        return ""
    }
    return stackView.currentItem.objectName || ""
}
