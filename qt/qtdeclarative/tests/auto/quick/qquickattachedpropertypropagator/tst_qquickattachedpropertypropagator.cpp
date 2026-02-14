// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtQuickTestUtils/private/qmlutils_p.h>
#include <QtQuickTestUtils/private/visualtestutils_p.h>
#include <QtQuick/private/qquickattachedpropertypropagator_p.h>

using namespace QQuickVisualTestUtils;

// This tests the private copy of QQuickAttachedPropertyPropagator.
// tst_material.qml, tst_universal.qml and tst_imagine.qml (to a lesser extent)
// test the public type in Controls.
class tst_QQuickAttachedPropertyPropagator : public QQmlDataTest
{
    Q_OBJECT

public:
    tst_QQuickAttachedPropertyPropagator();

private slots:
    void topLevelPopupNoOverride();
    void topLevelPopupOverride();
    void attachedParent();
    void reparenting();
};

static const QString fooDefault = "default";
static const bool doNotCreate = false;
static const bool create = true;

class Attached : public QtPrivate::QQuickAttachedPropertyPropagator
{
    Q_OBJECT
    Q_PROPERTY(QString foo READ foo WRITE setFoo RESET resetFoo NOTIFY fooChanged FINAL)
    QML_ELEMENT
    QML_ATTACHED(Attached)
    QML_UNCREATABLE("")

public:
    Attached(QObject *parent = nullptr);

    static Attached *qmlAttachedProperties(QObject *object);

    QString foo() const;
    void setFoo(const QString &foo);
    void resetFoo();
    void inheritFoo(const QString &foo);
    void propagateFoo();

    static Attached *attachedObject(const QObject *object, bool create = doNotCreate);

Q_SIGNALS:
    void fooChanged();

protected:
    void attachedParentChange(QQuickAttachedPropertyPropagator *newParent, QQuickAttachedPropertyPropagator *oldParent) override;

private:
    QString m_foo = fooDefault;
    bool m_explicitFoo = false;
};

Attached::Attached(QObject *parent)
    : QQuickAttachedPropertyPropagator(parent)
{
    initialize();
}

Attached *Attached::qmlAttachedProperties(QObject *object)
{
    return new Attached(object);
}

QString Attached::foo() const
{
    return m_foo;
}

void Attached::setFoo(const QString &foo)
{
    m_explicitFoo = true;
    if (m_foo == foo)
        return;

    m_foo = foo;
    propagateFoo();
    emit fooChanged();
}

void Attached::resetFoo()
{
    if (!m_explicitFoo)
        return;

    m_explicitFoo = false;
    auto *attachedParentFoo = qobject_cast<Attached *>(attachedParent());
    inheritFoo(attachedParentFoo ? attachedParentFoo->foo() : fooDefault);
    emit fooChanged();
}

void Attached::inheritFoo(const QString &foo)
{
    if (m_explicitFoo || m_foo == foo)
        return;

    m_foo = foo;
    propagateFoo();
    emit fooChanged();
}

void Attached::propagateFoo()
{
    const auto attachedChildrenList = attachedChildren();
    for (QQuickAttachedPropertyPropagator *child : attachedChildrenList) {
        if (Attached *attachedChild = qobject_cast<Attached *>(child)) {
            attachedChild->inheritFoo(m_foo);
        }
    }
}

Attached *Attached::attachedObject(const QObject *object, bool create)
{
    auto *attached = qmlAttachedPropertiesObject<Attached>(object, create);
    return static_cast<Attached *>(attached);
}

void Attached::attachedParentChange(QQuickAttachedPropertyPropagator *newParent, QQuickAttachedPropertyPropagator *oldParent)
{
    Q_UNUSED(oldParent)

    Attached *attachedParentFoo = qobject_cast<Attached *>(newParent);
    if (attachedParentFoo) {
        inheritFoo(attachedParentFoo->foo());
    } else {
        resetFoo();
    }
}

tst_QQuickAttachedPropertyPropagator::tst_QQuickAttachedPropertyPropagator()
    : QQmlDataTest(QT_QMLTEST_DATADIR)
{
    qmlRegisterTypesAndRevisions<Attached>("Test", 1);
}

void tst_QQuickAttachedPropertyPropagator::topLevelPopupNoOverride()
{
    QQuickApplicationHelper helper(this, "topLevelPopupNoOverride.qml");
    QVERIFY2(helper.ready, helper.failureMessage());
    QQuickWindow *window = helper.window;
    window->show();
    QVERIFY(QTest::qWaitForWindowExposed(window));

    // Check values at start-up.
    auto *windowAttached = Attached::attachedObject(window);
    QVERIFY(windowAttached);
    QCOMPARE(windowAttached->foo(), "1");

    auto *topLevelPopupNoOverride = window->findChild<QObject *>("topLevelPopupNoOverride");
    QVERIFY(topLevelPopupNoOverride);
    QVERIFY(!Attached::attachedObject(topLevelPopupNoOverride));

    auto *noOverrideChildPopup = window->findChild<QObject *>("noOverrideChildPopup");
    QVERIFY(noOverrideChildPopup);
    QVERIFY(!Attached::attachedObject(noOverrideChildPopup));

    auto *noOverrideChildItem = window->findChild<QObject *>("noOverrideChildItem");
    QVERIFY(noOverrideChildItem);
    QVERIFY(!Attached::attachedObject(noOverrideChildItem));

    auto *overrideChildPopup = window->findChild<QObject *>("overrideChildPopup");
    QVERIFY(overrideChildPopup);
    auto *overrideChildPopupAttached = Attached::attachedObject(overrideChildPopup);
    QVERIFY(overrideChildPopupAttached);
    QCOMPARE(overrideChildPopupAttached->foo(), "2.3");

    auto *overrideChildItem = window->findChild<QObject *>("overrideChildItem");
    QVERIFY(overrideChildItem);
    auto *overrideChildItemAttached = Attached::attachedObject(overrideChildItem);
    QVERIFY(overrideChildItemAttached);
    QCOMPARE(overrideChildItemAttached->foo(), "2.4");

    // Check that creating the attached properties where they didn't exist results in values inherited from parents.
    auto *topLevelPopupNoOverrideAttached = Attached::attachedObject(topLevelPopupNoOverride, create);
    QCOMPARE(topLevelPopupNoOverrideAttached->foo(), fooDefault);
    auto *noOverrideChildPopupAttached = Attached::attachedObject(noOverrideChildPopup, create);
    QCOMPARE(noOverrideChildPopupAttached->foo(), fooDefault);
    auto *noOverrideChildItemAttached = Attached::attachedObject(noOverrideChildItem, create);
    QCOMPARE(noOverrideChildItemAttached->foo(), fooDefault);

    // Check that overriding the top level popup changes values for child items without overrides.
    topLevelPopupNoOverrideAttached->setFoo("2");
    // Popups do not propagate attached properties to child popups; see "Property Propagation" in Popup's docs.
    QCOMPARE(noOverrideChildPopupAttached->foo(), fooDefault);
    QCOMPARE(noOverrideChildItemAttached->foo(), "2");
}

void tst_QQuickAttachedPropertyPropagator::topLevelPopupOverride()
{
    QQuickApplicationHelper helper(this, "topLevelPopupOverride.qml");
    QVERIFY2(helper.ready, helper.failureMessage());
    QQuickWindow *window = helper.window;
    window->show();
    QVERIFY(QTest::qWaitForWindowExposed(window));

    // Check values at start-up.
    auto *windowAttached = Attached::attachedObject(window);
    QVERIFY(windowAttached);
    QCOMPARE(windowAttached->foo(), "1");

    auto *topLevelPopupOverride = window->findChild<QObject *>("topLevelPopupOverride");
    QVERIFY(topLevelPopupOverride);
    auto *topLevelPopupOverrideAttached = Attached::attachedObject(topLevelPopupOverride);
    QVERIFY(topLevelPopupOverrideAttached);
    QCOMPARE(topLevelPopupOverrideAttached->foo(), "2");

    auto *noOverrideChildPopup = window->findChild<QObject *>("noOverrideChildPopup");
    QVERIFY(noOverrideChildPopup);
    auto *noOverrideChildPopupAttached = Attached::attachedObject(noOverrideChildPopup, create);
    // Popups do not propagate attached properties to child popups.
    QCOMPARE(noOverrideChildPopupAttached->foo(), fooDefault);

    auto *noOverrideChildItem = window->findChild<QObject *>("noOverrideChildItem");
    QVERIFY(noOverrideChildItem);
    QVERIFY(!Attached::attachedObject(noOverrideChildItem));
    auto *noOverrideChildItemAttached = Attached::attachedObject(noOverrideChildItem, create);
    QCOMPARE(noOverrideChildItemAttached->foo(), "2");

    auto *overrideChildPopup = window->findChild<QObject *>("overrideChildPopup");
    QVERIFY(overrideChildPopup);
    auto *overrideChildPopupAttached = Attached::attachedObject(overrideChildPopup);
    QVERIFY(overrideChildPopupAttached);
    QCOMPARE(overrideChildPopupAttached->foo(), "2.3");

    auto *overrideChildItem = window->findChild<QObject *>("overrideChildItem");
    QVERIFY(overrideChildItem);
    auto *overrideChildItemAttached = Attached::attachedObject(overrideChildItem);
    QVERIFY(overrideChildItemAttached);
    QCOMPARE(overrideChildItemAttached->foo(), "2.4");

    // Reset foo on topLevelPopupOverride and check that overridden properties don't change,
    // and that inherited ones do.
    topLevelPopupOverrideAttached->resetFoo();
    QCOMPARE(noOverrideChildPopupAttached->foo(), fooDefault);
    QCOMPARE(noOverrideChildItemAttached->foo(), fooDefault);
    QCOMPARE(overrideChildPopupAttached->foo(), "2.3");
    QCOMPARE(overrideChildItemAttached->foo(), "2.4");
}

void tst_QQuickAttachedPropertyPropagator::attachedParent()
{
    QQuickApplicationHelper helper(this, "topLevelPopupOverride.qml");
    QVERIFY2(helper.ready, helper.failureMessage());
    QQuickWindow *window = helper.window;
    window->show();
    QVERIFY(QTest::qWaitForWindowExposed(window));

    auto *windowAttached = Attached::attachedObject(window);
    QVERIFY(windowAttached);
    QCOMPARE(windowAttached->foo(), "1");
    auto *windowAttachedParent = windowAttached->attachedParent();
    QVERIFY(windowAttachedParent);
    auto *fallbackAttached = QtPrivate::QQuickAttachedPropertyPropagator::fallbackAttachedObject(
        &Attached::staticMetaObject, window);
    QVERIFY(fallbackAttached);
    fallbackAttached->setObjectName("fallbackAttached");
    // The fallback attached object is attached to the engine.
    QCOMPARE(fallbackAttached->parent(), &helper.engine);
    QCOMPARE(windowAttachedParent, fallbackAttached);

    auto *topLevelPopupOverride = window->findChild<QObject *>("topLevelPopupOverride");
    QVERIFY(topLevelPopupOverride);
    auto *topLevelPopupOverrideAttached = Attached::attachedObject(topLevelPopupOverride);
    QVERIFY(topLevelPopupOverrideAttached);
    QVERIFY(topLevelPopupOverrideAttached->attachedParent());
    // Popups do not propagate attached properties to child popups, so their attached parents
    // should be the window. However, popups only get a window when they become visible. Until that
    // point, they will fall back to the engine's attached object.
    QCOMPARE(topLevelPopupOverrideAttached->attachedParent(), fallbackAttached);

    auto *noOverrideChildPopup = window->findChild<QObject *>("noOverrideChildPopup");
    QVERIFY(noOverrideChildPopup);
    auto *noOverrideChildPopupAttached = Attached::attachedObject(noOverrideChildPopup, create);
    QVERIFY(noOverrideChildPopupAttached->attachedParent());
    QCOMPARE(noOverrideChildPopupAttached->attachedParent(), fallbackAttached);

    auto *noOverrideChildItem = window->findChild<QObject *>("noOverrideChildItem");
    QVERIFY(noOverrideChildItem);
    auto *noOverrideChildItemAttached = Attached::attachedObject(noOverrideChildItem, create);
    QVERIFY(noOverrideChildItemAttached->attachedParent());
    QCOMPARE(noOverrideChildItemAttached->attachedParent(), topLevelPopupOverrideAttached);

    auto *overrideChildPopup = window->findChild<QObject *>("overrideChildPopup");
    QVERIFY(overrideChildPopup);
    auto *overrideChildPopupAttached = Attached::attachedObject(overrideChildPopup);
    QVERIFY(overrideChildPopupAttached);
    QVERIFY(overrideChildPopupAttached->attachedParent());
    QCOMPARE(overrideChildPopupAttached->attachedParent(), fallbackAttached);

    auto *overrideChildItem = window->findChild<QObject *>("overrideChildItem");
    QVERIFY(overrideChildItem);
    auto *overrideChildItemAttached = Attached::attachedObject(overrideChildItem);
    QVERIFY(overrideChildItemAttached);
    QVERIFY(overrideChildItemAttached->attachedParent());
    // overrideChildItem gets its attached object before topLevelPopup does,
    // so findAttachedParent doesn't return the popup because it has no attached object by that
    // point. If topLevelPopup were a component that had Attached bindings (such as e.g. Material's
    // Popup does), those would get evaluated first and overrideChildItem's attached parent would
    // be topLevelPopupOverrideAttached. Without that, it only changes after the popup is shown.
    // Typically this isn't a problem, as most attached properties are related to visual stuff,
    // and all that matters in that context is that everything looks right when the popup is shown.
    QCOMPARE(overrideChildItemAttached->attachedParent(), fallbackAttached);

    // Open popup so that they get windows. Popups do not propagate attached properties to child
    // popups, so their attached parents should be the window.
    QVERIFY(topLevelPopupOverride->setProperty("visible", true));
    QCOMPARE(topLevelPopupOverrideAttached->attachedParent(), windowAttached);
    QVERIFY(noOverrideChildPopup->setProperty("visible", true));
    QCOMPARE(noOverrideChildPopupAttached->attachedParent(), windowAttached);
    QVERIFY(overrideChildPopup->setProperty("visible", true));
    QCOMPARE(overrideChildPopupAttached->attachedParent(), windowAttached);
    QCOMPARE(overrideChildItemAttached->attachedParent(), topLevelPopupOverrideAttached);
}

void tst_QQuickAttachedPropertyPropagator::reparenting()
{
    QQuickApplicationHelper helper(this, "reparenting.qml");
    QVERIFY2(helper.ready, helper.failureMessage());
    QQuickWindow *window = helper.window;
    window->show();
    QVERIFY(QTest::qWaitForWindowExposed(window));

    auto *windowAttached = Attached::attachedObject(window);
    QVERIFY(windowAttached);
    QCOMPARE(windowAttached->foo(), "W");

    auto *i0 = window->findChild<QQuickItem *>("i0");
    QVERIFY(i0);
    auto *i0Attached = Attached::attachedObject(i0);
    QVERIFY(i0Attached);
    QCOMPARE(i0Attached->foo(), "W/0");

    auto *i1 = window->findChild<QQuickItem *>("i1");
    QVERIFY(i1);

    QQuickItem *i3 = window->findChild<QQuickItem *>("i3");
    QVERIFY(i3);
    auto *i3Attached = Attached::attachedObject(i3);
    QVERIFY(i3Attached);
    QCOMPARE(i3Attached->foo(), "W/0");

    auto *i5 = window->findChild<QQuickItem *>("i5");
    QVERIFY(i5);
    auto *i5Attached = Attached::attachedObject(i5);
    QVERIFY(i5Attached);
    QCOMPARE(i5Attached->foo(), "W/0");

    auto *i6 = window->findChild<QQuickItem *>("i6");
    QVERIFY(i6);
    auto *i6Attached = Attached::attachedObject(i6);
    QVERIFY(i6Attached);
    QCOMPARE(i6Attached->foo(), "W/0/1/3/6");

    // ---- Setup and sanity precondition tests done ----

    // reparent i1 away from from i0. => i1 is root of its own subtree:
    i1->setParentItem(nullptr);

    // items that got their content propagated from i0 will have new values
    QCOMPARE(i5Attached->foo(), fooDefault);

    i3Attached->setFoo("W/0/3");
    QCOMPARE(i5Attached->foo(), "W/0/3");

    i3Attached->resetFoo();
    QCOMPARE(i5Attached->foo(), fooDefault);

    // no propagation from i0 should happen
    i0Attached->setFoo("WOOF");
    QCOMPARE(i3Attached->foo(), fooDefault);

    // reparent i1 back as a child of i0
    i1->setParentItem(i0);

    // 1. Check if items inherit the new value of i0Attached ("WOOF")
    QCOMPARE(i3Attached->foo(), "WOOF");
    QCOMPARE(i5Attached->foo(), "WOOF");
    QCOMPARE(i6Attached->foo(), "W/0/1/3/6");
}

QTEST_MAIN(tst_QQuickAttachedPropertyPropagator)

#include "tst_qquickattachedpropertypropagator.moc"
