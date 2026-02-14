// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/qtest.h>
#include <QtTest/qsignalspy.h>
#include <QtGui/qstylehints.h>
#include <QtQml/qqmlcomponent.h>
#include <QtQml/qqmlengine.h>
#include <QtQuick/qquickwindow.h>
#include <QtQuick/qquickitem.h>
#include <QtQuickTestUtils/private/visualtestutils_p.h>
#include <QtQuickControlsTestUtils/private/controlstestutils_p.h>
#include <QtQuickControlsTestUtils/private/qtest_quickcontrols_p.h>
#include <QtQuickTemplates2/private/qquickabstractbutton_p.h>
#include <QtQuickTemplates2/private/qquicktooltip_p.h>
#include <QtQuickTemplates2/private/qquicktooltip_p_p.h>

using namespace QQuickControlsTestUtils;
using namespace QQuickVisualTestUtils;

#if defined(QT_BUILD_INTERNAL)
QT_BEGIN_NAMESPACE
extern Q_AUTOTEST_EXPORT bool qt_quicktooltipattachedprivate_short_timeout;
static constexpr int shortTimeout = 123;
extern Q_AUTOTEST_EXPORT int qt_quicktooltipattachedprivate_delay;
QT_END_NAMESPACE
#endif

class tst_QQuickToolTip : public QQmlDataTest
{
    Q_OBJECT

public:
    tst_QQuickToolTip();

private slots:
    void initTestCase() override;
    void init() override;
    void visible_data();
    void visible();
    void delayValues_data();
    void delayValues();
    void timeoutValues_data();
    void timeoutValues();
    void automaticDelayAndTimeout();
};

enum EventType {
    Mouse,
    Touch
};

enum class PropertyState {
    Unset,
    Set
};

tst_QQuickToolTip::tst_QQuickToolTip()
    : QQmlDataTest(QT_QMLTEST_DATADIR)
{
#if defined(QT_BUILD_INTERNAL)
    // The minimum timeout is 10 seconds, and we don't want to wait that long.
    qt_quicktooltipattachedprivate_short_timeout = true;
    qt_quicktooltipattachedprivate_delay = 50;
#endif
}

void tst_QQuickToolTip::initTestCase()
{
    QQmlDataTest::initTestCase();

    QTest::defaultTryTimeout.store(std::chrono::seconds(1));
    // Running this for all styles isn't really necessary as the bulk of what we want to test
    // is the logic, and the visible test alone takes ~70 seconds even with
    // QUnifiedTimer::instance()->setSpeedModifier(10).
    // Use tst_tooltip.qml for tests that should be run for all styles.
    qputenv("QT_QUICK_CONTROLS_STYLE", "Basic");
}

void tst_QQuickToolTip::init()
{
    QQmlDataTest::init();

    QTest::failOnWarning();
}

static const QQuickToolTip::Policy Automatic = QQuickToolTip::Automatic;
static const QQuickToolTip::Policy Manual = QQuickToolTip::Manual;

void tst_QQuickToolTip::visible_data()
{
    QTest::addColumn<QString>("controlType");
    QTest::addColumn<EventType>("eventType");
    QTest::addColumn<QQuickToolTip::Policy>("policy");
    QTest::addColumn<PropertyState>("visibleState");
    QTest::addColumn<QString>("text");
    QTest::addColumn<bool>("expectVisible");

    static const QString text = "Test";
    static const bool expectVisible = true;
    static const bool expectNotVisible = false;

    QTest::newRow("Control, mouse, automatic, visible set")
        << "Control" << Mouse << Automatic << PropertyState::Set << text << expectNotVisible;
    QTest::newRow("Control, mouse, automatic, visible unset")
        << "Control" << Mouse << Automatic << PropertyState::Unset << text << expectVisible;
    QTest::newRow("Control, mouse, automatic, visible unset, no text")
        << "Control" << Mouse << Automatic << PropertyState::Unset << QString() << expectNotVisible;
    QTest::newRow("Control, mouse, manual, visible set")
        << "Control" << Mouse << Manual << PropertyState::Set << text << expectNotVisible;
    QTest::newRow("Control, mouse, manual, visible unset")
        << "Control" << Mouse << Manual << PropertyState::Unset << text << expectNotVisible;
    QTest::newRow("Control, mouse, manual, visible unset, no text")
        << "Control" << Mouse << Manual << PropertyState::Unset << QString() << expectNotVisible;

    // Non-AbstractButton types have no notion of pressAndHold, so we can't test touch for them.
    // Android only shows tooltips on long press for buttons.
    QTest::newRow("AbstractButton, mouse, automatic, visible set")
        << "AbstractButton" << Mouse << Automatic << PropertyState::Set << text << expectNotVisible;
    QTest::newRow("AbstractButton, mouse, automatic, visible unset")
        << "AbstractButton" << Mouse << Automatic << PropertyState::Unset << text << expectVisible;
    QTest::newRow("AbstractButton, touch, automatic, visible set")
        << "AbstractButton" << Touch << Automatic << PropertyState::Set << text << expectNotVisible;
    QTest::newRow("AbstractButton, touch, automatic, visible unset")
        << "AbstractButton" << Touch << Automatic << PropertyState::Unset << text << expectVisible;
    QTest::newRow("AbstractButton, touch, automatic, visible unset, no text")
        << "AbstractButton" << Touch << Automatic << PropertyState::Unset << QString() << expectNotVisible;
    QTest::newRow("AbstractButton, mouse, manual, visible set")
        << "AbstractButton" << Mouse << Manual << PropertyState::Set << text << expectNotVisible;
    QTest::newRow("AbstractButton, mouse, manual, visible unset")
        << "AbstractButton" << Mouse << Manual << PropertyState::Unset << text << expectNotVisible;
    QTest::newRow("AbstractButton, touch, manual, visible set")
        << "AbstractButton" << Touch << Manual << PropertyState::Set << text << expectNotVisible;
    QTest::newRow("AbstractButton, touch, manual, visible unset")
        << "AbstractButton" << Touch << Manual << PropertyState::Unset << text << expectNotVisible;
    QTest::newRow("AbstractButton, touch, manual, visible unset, no text")
        << "AbstractButton" << Touch << Manual << PropertyState::Unset << QString() << expectNotVisible;

    QTest::newRow("TextField, mouse, automatic, visible set")
        << "TextField" << Mouse << Automatic << PropertyState::Set << text << expectNotVisible;
    QTest::newRow("TextField, mouse, automatic, visible unset")
        << "TextField" << Mouse << Automatic << PropertyState::Unset << text << expectVisible;
    QTest::newRow("TextField, mouse, automatic, visible unset, no text")
        << "TextField" << Mouse << Automatic << PropertyState::Unset << QString() << expectNotVisible;
    QTest::newRow("TextField, mouse, manual, visible set")
        << "TextField" << Mouse << Manual << PropertyState::Set << text << expectNotVisible;
    QTest::newRow("TextField, mouse, manual, visible unset")
        << "TextField" << Mouse << Manual << PropertyState::Unset << text << expectNotVisible;
    QTest::newRow("TextField, mouse, manual, visible unset, no text")
        << "TextField" << Mouse << Manual << PropertyState::Unset << QString() << expectNotVisible;

    QTest::newRow("TextArea, mouse, automatic, visible set")
        << "TextArea" << Mouse << Automatic << PropertyState::Set << text << expectNotVisible;
    QTest::newRow("TextArea, mouse, automatic, visible unset")
        << "TextArea" << Mouse << Automatic << PropertyState::Unset << text << expectVisible;
    QTest::newRow("TextArea, mouse, automatic, visible unset, no text")
        << "TextArea" << Mouse << Automatic << PropertyState::Unset << QString() << expectNotVisible;
    QTest::newRow("TextArea, mouse, manual, visible set")
        << "TextArea" << Mouse << Manual << PropertyState::Set << text << expectNotVisible;
    QTest::newRow("TextArea, mouse, manual, visible unset")
        << "TextArea" << Mouse << Manual << PropertyState::Unset << text << expectNotVisible;
    QTest::newRow("TextArea, mouse, manual, visible unset, no text")
        << "TextArea" << Mouse << Manual << PropertyState::Unset << QString() << expectNotVisible;
}

void tst_QQuickToolTip::visible()
{
#if defined(QT_BUILD_INTERNAL)
    QFETCH(QString, controlType);
    QFETCH(EventType, eventType);
    QFETCH(QQuickToolTip::Policy, policy);
    QFETCH(PropertyState, visibleState);
    QFETCH(QString, text);
    QFETCH(bool, expectVisible);

    QQmlEngine engine;
    QQmlComponent component(&engine);
    const bool automatic = policy == Automatic;
    const QByteArray qml = qPrintable(QString::fromLatin1(
        "import QtQuick\n"
        "import QtQuick.Controls\n"
        "ApplicationWindow {\n"
            "width: 400\n"
            "height: 400\n"
            "ToolTip.policy: ToolTip.%1\n"
            "%2 {\n"
                "objectName: '%3'\n"
                "width: 100\n"
                "height: 100\n"
                "hoverEnabled: %4\n"
                "anchors.centerIn: parent\n"
                "ToolTip.text: '%5'\n"
                "Rectangle {\n" // Just for visually debugging hover.
                    "anchors.fill: parent\n"
                    "color: parent.hovered ? 'salmon' : 'steelblue'\n"
                "}\n"
            "}\n"
        "}")
        .arg(automatic ? "Automatic" : "Manual",
            controlType,
            QTest::currentDataTag(),
            eventType == Mouse ? "true" : "false", text));
    component.setData(qml, QUrl());
    QVERIFY2(component.isReady(), qPrintable(component.errorString()));

    std::unique_ptr<QQuickWindow> window(qobject_cast<QQuickWindow *>(component.create()));
    QVERIFY2(window, qPrintable(component.errorString()));
    window->show();
    QVERIFY(QTest::qWaitForWindowExposed(window.get()));

    auto *control(window->findChild<QQuickItem *>(QTest::currentDataTag()));
    QVERIFY(control);

    auto *toolTipAttached = qobject_cast<QQuickToolTipAttached *>(
        qmlAttachedPropertiesObject<QQuickToolTip>(control));
    QVERIFY(toolTipAttached);
    auto *toolTipAttachedPrivate = QQuickToolTipAttachedPrivate::get(toolTipAttached);
    QCOMPARE(toolTipAttached->policy(), policy);

    // Only set this if visibleSet is true, because the logic relies on its explicitly set
    // state. Setting visible to false is weird, but makes the test faster (don't need to wait
    // for it to become visible) and the value shouldn't matter anyway, as we want to respect
    // the user's visibility logic, regardless of what it is.
    if (visibleState == PropertyState::Set)
        toolTipAttached->setVisible(false);
    QCOMPARE(toolTipAttachedPrivate->isVisibleExplicitlySet(), visibleState == PropertyState::Set);

    const QPoint controlCenter = mapCenterToWindow(control);
    // Interact with the control to potentially show the tool tip.
    std::unique_ptr<QPointingDevice> touchDevice(QTest::createTouchDevice());
    if (eventType == Mouse) {
        PointLerper(window.get()).move(controlCenter);
        QVERIFY(control->property("hovered").toBool());
    } else {
        qGuiApp->styleHints()->setMousePressAndHoldInterval(50);

        QSignalSpy pressAndHoldSpy(qobject_cast<QQuickAbstractButton *>(control),
            &QQuickAbstractButton::pressAndHold);
        QVERIFY(pressAndHoldSpy.isValid());

        QTest::touchEvent(window.get(), touchDevice.get()).press(0, controlCenter);
        QVERIFY(pressAndHoldSpy.wait());
    }

    // Check that its explicit state hasn't changed since the event.
    QCOMPARE(toolTipAttachedPrivate->isVisibleExplicitlySet(), visibleState == PropertyState::Set);

    // Check that the tool tip's visibility is what we expect.
    if (expectVisible) {
        QTRY_VERIFY(toolTipAttached->toolTip()->isOpened());
        QVERIFY(!toolTipAttachedPrivate->isVisibleExplicitlySet());

        // Check that it's positioned correctly.
        QCOMPARE(toolTipAttached->toolTip()->x(),
            (control->width() - toolTipAttached->toolTip()->implicitWidth()) / 2);
        // The y position varies across styles, but all styles position it above the control.
        // The check below verifies that its bottom edges sits at or above the top edge of the control.
        QCOMPARE_LE(toolTipAttached->toolTip()->y(), -toolTipAttached->toolTip()->implicitHeight());
        // As a sanity check, verify that it's within the bounds of the window.
        QCOMPARE_GE(toolTipAttached->toolTip()->y() + control->y(), 0);
    } else {
        QVERIFY(!toolTipAttached->isVisible());
    }

    // End the interaction that caused the tool tip to be shown.
    if (eventType == Mouse) {
        // Move the mouse away. Don't go to {0, 0}, otherwise it becomes hovered again for some reason.
        PointLerper(window.get(), controlCenter).move({1, 1});
        QVERIFY(!control->property("hovered").toBool());
    } else {
        // Release the long press.
        QSignalSpy releasedSpy(qobject_cast<QQuickAbstractButton *>(control),
            &QQuickAbstractButton::released);
        QVERIFY(releasedSpy.isValid());

        QTest::touchEvent(window.get(), touchDevice.get()).release(0, controlCenter);
        QCOMPARE(releasedSpy.size(), 1);
    }

    // Check that it's eventually hidden.
    QTRY_VERIFY(!toolTipAttached->toolTip()->isVisible());
    QCOMPARE(toolTipAttachedPrivate->isVisibleExplicitlySet(), visibleState == PropertyState::Set);

    // As an extra check for mouse, verify that pressing the button while it's hovered
    // causes the tool tip to close. This matches Widgets' behavior.
    if (automatic && controlType == "AbstractButton" && eventType == Mouse
            && visibleState == PropertyState::Unset && !text.isEmpty()) {
        // Make it visible by hovering the button.
        PointLerper(window.get()).move(controlCenter);
        QVERIFY(control->property("hovered").toBool());
        QTRY_VERIFY(toolTipAttached->toolTip()->isOpened());

        // Press; it should close.
        QTest::mousePress(window.get(), Qt::LeftButton, Qt::NoModifier, controlCenter);
        QTRY_VERIFY(!toolTipAttached->toolTip()->isVisible());
        QTest::mouseRelease(window.get(), Qt::LeftButton, Qt::NoModifier, controlCenter);
    }
#else
    QSKIP("This test relies on private APIs that are only exported in developer builds");
#endif
}

void tst_QQuickToolTip::delayValues_data()
{
    QTest::addColumn<QQuickToolTip::Policy>("policy");
    QTest::addColumn<PropertyState>("delayState");
    QTest::addColumn<int>("expectedDelay");

    static const int userDelay = 123;
    static const int defaultManualDelay = 0;

    QTest::newRow("automatic, delay set")
        << Automatic << PropertyState::Set << userDelay;
    QTest::newRow("automatic, delay unset")
        << Automatic << PropertyState::Unset
#if defined(QT_BUILD_INTERNAL)
        << qt_quicktooltipattachedprivate_delay;
#else
        // Doesn't matter since it will be skipped; this is just to get it to build. Also,
        // if we ifdef out all of these rows instead, -datags won't show our tags.
        << 0;
#endif
    QTest::newRow("manual, delay set")
        << Manual << PropertyState::Set << userDelay;
    QTest::newRow("manual, delay unset")
        << Manual << PropertyState::Unset << defaultManualDelay;
}

// This just tests the property's values; actual delay logic is tested in tst_tooltip.qml.
void tst_QQuickToolTip::delayValues()
{
#if defined(QT_BUILD_INTERNAL)
    QFETCH(QQuickToolTip::Policy, policy);
    QFETCH(PropertyState, delayState);
    QFETCH(int, expectedDelay);

    QQmlEngine engine;
    QQmlComponent component(&engine);
    const bool automatic = policy == Automatic;
    const QByteArray qml = qPrintable(QString::fromLatin1(
        "import QtQuick\n"
        "import QtQuick.Controls\n"
        "ApplicationWindow {\n"
            "width: 400\n"
            "height: 400\n"
            "ToolTip.policy: ToolTip.%1\n"
            "Button {\n"
                "objectName: '%2'\n"
                "width: 100\n"
                "height: 100\n"
                "hoverEnabled: true\n"
                "anchors.centerIn: parent\n"
                "ToolTip.text: 'Some text'\n"
                "Rectangle {\n" // Just for visually debugging hover.
                    "anchors.fill: parent\n"
                    "color: parent.hovered ? 'salmon' : 'steelblue'\n"
                "}\n"
            "}\n"
        "}")
        .arg(automatic ? "Automatic" : "Manual", QTest::currentDataTag()));
    component.setData(qml, QUrl());
    QVERIFY2(component.isReady(), qPrintable(component.errorString()));

    std::unique_ptr<QQuickWindow> window(qobject_cast<QQuickWindow *>(component.create()));
    QVERIFY2(window, qPrintable(component.errorString()));
    window->show();
    QVERIFY(QTest::qWaitForWindowExposed(window.get()));

    auto *control(window->findChild<QQuickItem *>(QTest::currentDataTag()));
    QVERIFY(control);

    auto *toolTipAttached = qobject_cast<QQuickToolTipAttached *>(
        qmlAttachedPropertiesObject<QQuickToolTip>(control));
    QVERIFY(toolTipAttached);
    auto *toolTipAttachedPrivate = QQuickToolTipAttachedPrivate::get(toolTipAttached);
    QCOMPARE(toolTipAttached->policy(), policy);

    // Only set this if visibleSet is true, because the logic relies on its explicitly set state.
    if (delayState == PropertyState::Set)
        toolTipAttached->setDelay(expectedDelay);
    QCOMPARE(toolTipAttachedPrivate->isDelayExplicitlySet(), delayState == PropertyState::Set);
    QCOMPARE(toolTipAttached->delay(), expectedDelay);
#else
    QSKIP("This test relies on private APIs that are only exported in developer builds");
#endif
}

void tst_QQuickToolTip::timeoutValues_data()
{
    QTest::addColumn<QQuickToolTip::Policy>("policy");
    QTest::addColumn<PropertyState>("timeoutState");
    QTest::addColumn<int>("expectedTimeout");

    static const int userTimeout = 123;
    static const int defaultAutomaticTimeout
#if defined(QT_BUILD_INTERNAL)
        = QQuickToolTipAttachedPrivate::calculateTimeout("Some text");
#else
        = 0; // Doesn't matter since it will be skipped; this is just to get it to build.
#endif
    static const int defaultManualTimeout = -1;

    QTest::newRow("automatic, timeout set")
        << Automatic << PropertyState::Set << userTimeout;
    QTest::newRow("automatic, timeout unset")
        << Automatic << PropertyState::Unset << defaultAutomaticTimeout;
    QTest::newRow("manual, timeout set")
        << Manual << PropertyState::Set << userTimeout;
    QTest::newRow("manual, timeout unset")
        << Manual << PropertyState::Unset << defaultManualTimeout;
}

// This just tests the property's values; actual timeoutValues logic is tested in
// tst_tooltip.qml (manual) and automaticTimeout() (automatic).
void tst_QQuickToolTip::timeoutValues()
{
#if defined(QT_BUILD_INTERNAL)
    QFETCH(QQuickToolTip::Policy, policy);
    QFETCH(PropertyState, timeoutState);
    QFETCH(int, expectedTimeout);

    QQmlEngine engine;
    QQmlComponent component(&engine);
    const bool automatic = policy == Automatic;
    const QByteArray qml = qPrintable(QString::fromLatin1(
        "import QtQuick\n"
        "import QtQuick.Controls\n"
        "ApplicationWindow {\n"
            "width: 400\n"
            "height: 400\n"
            "ToolTip.policy: ToolTip.%1\n"
            "Button {\n"
                "objectName: '%2'\n"
                "width: 100\n"
                "height: 100\n"
                "hoverEnabled: true\n"
                "anchors.centerIn: parent\n"
                "ToolTip.text: 'Some text'\n"
                "Rectangle {\n" // Just for visually debugging hover.
                    "anchors.fill: parent\n"
                    "color: parent.hovered ? 'salmon' : 'steelblue'\n"
                "}\n"
            "}\n"
        "}")
        .arg(automatic ? "Automatic" : "Manual", QTest::currentDataTag()));
    component.setData(qml, QUrl());
    QVERIFY2(component.isReady(), qPrintable(component.errorString()));

    std::unique_ptr<QQuickWindow> window(qobject_cast<QQuickWindow *>(component.create()));
    QVERIFY2(window, qPrintable(component.errorString()));
    window->show();
    QVERIFY(QTest::qWaitForWindowExposed(window.get()));

    auto *control(window->findChild<QQuickItem *>(QTest::currentDataTag()));
    QVERIFY(control);

    auto *toolTipAttached = qobject_cast<QQuickToolTipAttached *>(
        qmlAttachedPropertiesObject<QQuickToolTip>(control));
    QVERIFY(toolTipAttached);
    auto *toolTipAttachedPrivate = QQuickToolTipAttachedPrivate::get(toolTipAttached);
    QCOMPARE(toolTipAttached->policy(), policy);

    // Only set this if visibleSet is true, because the logic relies on its explicitly set state.
    if (timeoutState == PropertyState::Set)
        toolTipAttached->setTimeout(expectedTimeout);
    QCOMPARE(toolTipAttachedPrivate->isTimeoutExplicitlySet(), timeoutState == PropertyState::Set);
    QCOMPARE(toolTipAttached->timeout(), expectedTimeout);
#else
    QSKIP("This test relies on private APIs that are only exported in developer builds");
#endif
}

void tst_QQuickToolTip::automaticDelayAndTimeout()
{
#if defined(QT_BUILD_INTERNAL)
    QQuickControlsApplicationHelper helper(this, "automaticDelayAndTimeout.qml");
    QVERIFY2(helper.ready, helper.failureMessage());

    QQuickWindow *window = helper.window;
    window->show();
    QVERIFY(QTest::qWaitForWindowExposed(window));

    // Get the attached object and check that the delay and timeout values are our shortened ones.
    auto *toolTipAttached = qobject_cast<QQuickToolTipAttached *>(
        qmlAttachedPropertiesObject<QQuickToolTip>(window->contentItem()));
    QVERIFY(toolTipAttached);
    QCOMPARE(toolTipAttached->delay(), qt_quicktooltipattachedprivate_delay);
    QCOMPARE(toolTipAttached->timeout(), shortTimeout);

    // Start timing so we know how long the actual delay is.
    QElapsedTimer delayElapsedTimer;
    delayElapsedTimer.start();

    // Show it and check that it has (eventually) opened.
    toolTipAttached->setVisible(true);
    QTest::qWait(1);
    // The delay controls when the process of showing the tool tip begins, and doesn't account
    // for any enter transitions tool tips may have. So these checks use visible rather than opened.
    QVERIFY(!toolTipAttached->toolTip()->isVisible());
    QTRY_VERIFY(toolTipAttached->toolTip()->isVisible());
    QCOMPARE_GE(delayElapsedTimer.elapsed(), qt_quicktooltipattachedprivate_delay);

    // Now that the delay has elapsed, start timing the actual timeout.
    QElapsedTimer timeoutElapsedTimer;
    timeoutElapsedTimer.start();

    // Account for tool tips with enter transitions and ensure that it opened properly.
    QTRY_VERIFY(toolTipAttached->toolTip()->isOpened());

    // Wait a bit to check that it's still open.
    QTest::qWait(30);
    QVERIFY(toolTipAttached->toolTip()->isOpened());

    // Check that it's closed.
    QTRY_VERIFY(!toolTipAttached->toolTip()->isVisible());
    // It's not possible to reliably check that the actual timeout is at
    // least as long as we expect because of timer precision/machine load, so add some lenience in.
    QCOMPARE_GE(timeoutElapsedTimer.elapsed(), shortTimeout / 2);
#else
    QSKIP("This test relies on private APIs that are only exported in developer builds");
#endif
}

QTEST_MAIN(tst_QQuickToolTip)

#include "tst_qquicktooltip.moc"
