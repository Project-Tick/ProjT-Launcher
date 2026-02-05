// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtQuickTest/quicktest.h>
#include <QtQuickTestUtils/private/visualtestutils_p.h>
#include <QtQuickControlsTestUtils/private/controlstestutils_p.h>
#include <QtQuickControls2/qquickstyle.h>

int main(int argc, char *argv[])
{
    QTEST_SET_MAIN_SOURCE_PATH
    qputenv("QML_NO_TOUCH_COMPRESSION", "1");
    // The tests were originally written before native menus existed,
    // and some of them try to open menus, which we can't test natively.
    QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuWindows);
    QQuickStyle::setStyle("Fusion");
    QQuickControlsTestUtils::QmlTestHelper setup;
    return quick_test_main_with_setup(argc, argv, "tst_controls::Fusion", TST_CONTROLS_DATA, &setup);
}
