// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick
import QtTest
import QtQuick.Controls

TestCase {
    id: testCase
    name: "Color"

    property color red: "red"
    property color green: "#00FF00"
    property color nope
    property color alphaRed: "#77FF0000"

    function test_transparent() {
        compare(Color.transparent("red", 0.2), Qt.rgba(1, 0, 0, 0.2))
        compare(Color.transparent(Qt.rgba(0, 1, 0, 1), 0.2), Qt.rgba(0, 1, 0, 0.2))
        compare(Color.transparent("#0000ff", 0.2), Qt.rgba(0, 0, 1, 0.2))
    }

    property color c1: "#112233"
    property color c2: "#445566"
    function test_blend() {
        function checkBlend(c1, c2, factor, actual) {
            let compFactor = 1 - factor
            let r = (c1.r * compFactor) + c2.r * factor
            let g = (c1.g * compFactor) + c2.g * factor
            let b = (c1.b * compFactor) + c2.b * factor
            compare(actual.r, r)
            compare(actual.g, g)
            compare(actual.b, b)
        }

        checkBlend(c1, c2, 0.00, Color.blend(c1, c2, 0.00))
        checkBlend(c1, c2, 0.25, Color.blend(c1, c2, 0.25))
        checkBlend(c1, c2, 0.50, Color.blend(c1, c2, 0.50))
        checkBlend(c1, c2, 0.75, Color.blend(c1, c2, 0.75))
        checkBlend(c1, c2, 1.00, Color.blend(c1, c2, 1.00))

        compare(Color.blend(red, green, -1), red)
        compare(Color.blend(red, green, 2), green)
    }

    function test_rgba() {
        compare(Color.rgba(0, 0, 0, 0), Qt.rgba(0, 0, 0, 0))
        compare(Color.rgba(1, 1, 1, 1), Qt.rgba(1, 1, 1, 1))
        compare(Color.rgba(1, 0, 0, 1), Qt.rgba(1, 0, 0, 1))
        compare(Color.rgba(0.1, 0.2, 0.3, 0.4), Qt.rgba(0.1, 0.2, 0.3, 0.4))
    }

    function test_hsla() {
        compare(Color.hsla(0, 0, 0, 0), Qt.hsla(0, 0, 0, 0))
        compare(Color.hsla(1, 1, 1, 1), Qt.hsla(1, 1, 1, 1))
        compare(Color.hsla(1, 0, 0, 1), Qt.hsla(1, 0, 0, 1))
        compare(Color.hsla(0.1, 0.2, 0.3, 0.4), Qt.hsla(0.1, 0.2, 0.3, 0.4))
    }

    function test_hsva() {
        compare(Color.hsva(0, 0, 0, 0), Qt.hsva(0, 0, 0, 0))
        compare(Color.hsva(1, 1, 1, 1), Qt.hsva(1, 1, 1, 1))
        compare(Color.hsva(1, 0, 0, 1), Qt.hsva(1, 0, 0, 1))
        compare(Color.hsva(0.1, 0.2, 0.3, 0.4), Qt.hsva(0.1, 0.2, 0.3, 0.4))
    }

    function test_fromString() {
        compare(Color.fromString("red"), Qt.color("red"))
        compare(Color.fromString("#ff0000ff"), Qt.color("#ff0000ff"))

        let threw = false
        try {
            Color.fromString("")
        } catch(e) {
            threw = true
        }
        verify(threw)
    }

    function test_equal() {
        // true
        compare(Color.equal(red, red), Qt.colorEqual(red, red))
        compare(Color.equal(green, green), Qt.colorEqual(green, green))
        compare(Color.equal(nope, nope), Qt.colorEqual(nope, nope))
        compare(Color.equal(alphaRed, alphaRed), Qt.colorEqual(alphaRed, alphaRed))

        // false
        compare(Color.equal(red, green), Qt.colorEqual(red, green))
        compare(Color.equal(red, nope), Qt.colorEqual(red, nope))
        compare(Color.equal(alphaRed, red), Qt.colorEqual(alphaRed, red))
    }

    function test_alpha() {
        compare(Color.alpha(red, 0.0), Qt.alpha(red, 0.0))
        compare(Color.alpha(red, 0.5), Qt.alpha(red, 0.5))
        compare(Color.alpha(red, 1.0), Qt.alpha(red, 1.0))
        compare(Color.alpha(nope, 0.0), Qt.alpha(nope, 0.0))
        compare(Color.alpha(nope, 0.5), Qt.alpha(nope, 0.5))
        compare(Color.alpha(nope, 1.0), Qt.alpha(nope, 1.0))
        compare(Color.alpha(alphaRed, 0.0), Qt.alpha(alphaRed, 0.0))
        compare(Color.alpha(alphaRed, 0.5), Qt.alpha(alphaRed, 0.5))
        compare(Color.alpha(alphaRed, 1.0), Qt.alpha(alphaRed, 1.0))
    }

    function test_darker() {
        compare(Color.darker(red, 0.5), Qt.darker(red, 0.5))
        compare(Color.darker(red, 1.0), Qt.darker(red, 1.0))
        compare(Color.darker(red, 2.0), Qt.darker(red, 2.0))
        compare(Color.darker(nope, 0.5), Qt.darker(nope, 0.5))
        compare(Color.darker(nope, 1.0), Qt.darker(nope, 1.0))
        compare(Color.darker(nope, 2.0), Qt.darker(nope, 2.0))
        compare(Color.darker(alphaRed, 0.5), Qt.darker(alphaRed, 0.5))
        compare(Color.darker(alphaRed, 1.0), Qt.darker(alphaRed, 1.0))
        compare(Color.darker(alphaRed, 2.0), Qt.darker(alphaRed, 2.0))
    }

    function test_lighter() {
        compare(Color.lighter(red, 0.0), Qt.lighter(red, 0.0))
        compare(Color.lighter(red, 0.5), Qt.lighter(red, 0.5))
        compare(Color.lighter(red, 1.0), Qt.lighter(red, 1.0))
        compare(Color.lighter(nope, 0.0), Qt.lighter(nope, 0.0))
        compare(Color.lighter(nope, 0.5), Qt.lighter(nope, 0.5))
        compare(Color.lighter(nope, 1.0), Qt.lighter(nope, 1.0))
        compare(Color.lighter(alphaRed, 0.0), Qt.lighter(alphaRed, 0.0))
        compare(Color.lighter(alphaRed, 0.5), Qt.lighter(alphaRed, 0.5))
        compare(Color.lighter(alphaRed, 1.0), Qt.lighter(alphaRed, 1.0))
    }

    function test_tint() {
        compare(Color.tint(red, red), Qt.tint(red, red))
        compare(Color.tint(red, nope), Qt.tint(red, nope))
        compare(Color.tint(nope, red), Qt.tint(nope, red))
        compare(Color.tint(nope, nope), Qt.tint(nope, nope))

        compare(Color.tint(red, alphaRed), Qt.tint(red, alphaRed))
        compare(Color.tint(nope, alphaRed), Qt.tint(nope, alphaRed))
    }
}
