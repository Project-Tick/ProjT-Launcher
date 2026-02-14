// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qquickattachedpropertypropagator_p.h"
#include "qquickattachedpropertypropagator_p_p.h"
#include "qquickattachedpropertypropagatortarget_p.h"

#include <QtQuick/qquickwindow.h>
#include <QtQuick/private/qquickitem_p.h>

QT_BEGIN_NAMESPACE

Q_LOGGING_CATEGORY(lcAttachedPropertyPropagator, "qt.quick.private.attachedpropertypropagator")

namespace QtPrivate {

/*!
    \internal

    This is a copy of the type from Controls. We need it here
    so that QQuickContextMenuAttached can derive from it.

    ### Qt 7: remove the type in Controls and make this public: QTBUG-138546
*/

void QQuickAttachedPropertyPropagatorPrivate::init()
{
    attachTo(parent);
}

void QQuickAttachedPropertyPropagatorPrivate::attachTo(QObject *object)
{
    if (QQuickItem *item = findAttachedItem(object)) {
        connect(item, &QQuickItem::windowChanged, this, &QQuickAttachedPropertyPropagatorPrivate::itemWindowChanged);
        QQuickItemPrivate::get(item)->addItemChangeListener(this, QQuickItemPrivate::Parent);
    } else if (auto *window = qobject_cast<QQuickWindow *>(object)) {
        QObjectPrivate::connect(window, &QWindow::transientParentChanged, this,
            &QQuickAttachedPropertyPropagatorPrivate::transientParentWindowChanged);
    }
}

void QQuickAttachedPropertyPropagatorPrivate::detachFrom(QObject *object)
{
    if (QQuickItem *item = findAttachedItem(object)) {
        disconnect(item, &QQuickItem::windowChanged, this, &QQuickAttachedPropertyPropagatorPrivate::itemWindowChanged);
        QQuickItemPrivate::get(item)->removeItemChangeListener(this, QQuickItemPrivate::Parent);
    } else if (auto *window = qobject_cast<QQuickWindow *>(object)) {
        QObjectPrivate::disconnect(window, &QWindow::transientParentChanged,
            this, &QQuickAttachedPropertyPropagatorPrivate::transientParentWindowChanged);
    }
}

/*!
    \internal

    This function sets the attached parent of this attached object.

    Currently it is called when:
    \list
    \li The target item's parent changes.
    \li The target item's window changes.
    \li The attached object is constructed, to set the attached parent
        and the attached parent of the attached object children.
    \li The attached object is destructed.
    \endlist
*/
void QQuickAttachedPropertyPropagatorPrivate::setAttachedParent(QQuickAttachedPropertyPropagator *parent)
{
    Q_Q(QQuickAttachedPropertyPropagator);
    if (attachedParent == parent)
        return;

    QQuickAttachedPropertyPropagator *oldParent = attachedParent;
    qCDebug(lcAttachedPropertyPropagator).noquote() << "setAttachedParent called on" << q << "with parent" << parent;
    if (attachedParent) {
        qCDebug(lcAttachedPropertyPropagator).noquote() << "- removing ourselves as an attached child of" << attachedParent;
        QQuickAttachedPropertyPropagatorPrivate::get(attachedParent)->attachedChildren.removeOne(q);
    }
    attachedParent = parent;
    if (parent) {
        qCDebug(lcAttachedPropertyPropagator).noquote() << "- adding ourselves as an attached child of" << parent;
        QQuickAttachedPropertyPropagatorPrivate::get(parent)->attachedChildren.append(q);
    }
    q->attachedParentChange(parent, oldParent);
}

void QQuickAttachedPropertyPropagatorPrivate::itemWindowChanged(QQuickWindow *window)
{
    Q_Q(QQuickAttachedPropertyPropagator);
    qCDebug(lcAttachedPropertyPropagator).noquote().nospace() << "window of " << q << " changed to "
        << window << "...";
    QQuickAttachedPropertyPropagator *attachedParent = findAttachedParent(attacherMetaObject, q->parent());
    if (!attachedParent)
        attachedParent = QQuickAttachedPropertyPropagator::attachedObject(attacherMetaObject, window);
    setAttachedParent(attachedParent);
    qCDebug(lcAttachedPropertyPropagator).noquote()
        << "... finished handling parent window change of" << q;
}

void QQuickAttachedPropertyPropagatorPrivate::transientParentWindowChanged(QWindow *newTransientParent)
{
    Q_Q(QQuickAttachedPropertyPropagator);
    QQuickAttachedPropertyPropagator *attachedParent = nullptr;
    qCDebug(lcAttachedPropertyPropagator).noquote().nospace() << "transient parent window of "
        << q << " changed to " << newTransientParent << "...";
    attachedParent = findAttachedParent(attacherMetaObject, q->parent());
    if (!attachedParent) {
        attachedParent = QQuickAttachedPropertyPropagator::attachedObject(
            attacherMetaObject, newTransientParent);
    }
    setAttachedParent(attachedParent);
    qCDebug(lcAttachedPropertyPropagator).noquote()
        << "... finished handling transient parent window change of" << q;
}

void QQuickAttachedPropertyPropagatorPrivate::itemParentChanged(QQuickItem *item, QQuickItem *parent)
{
    Q_Q(QQuickAttachedPropertyPropagator);
    Q_UNUSED(item)
    Q_UNUSED(parent)
    qCDebug(lcAttachedPropertyPropagator).noquote().nospace() << "parent item of attachee " << item
        << " changed to " << parent << "; calling findAttachedParent()...";
    setAttachedParent(findAttachedParent(attacherMetaObject, q->parent()));
    qCDebug(lcAttachedPropertyPropagator).noquote()
        << "... finished handling item parent change of" << item;
}

/*
    If there's e.g. code like this:

        Behavior on Material.elevation {}

    The meta type will be something like QQuickMaterialStyle_QML_125,
    whereas QQmlMetaType::attachedPropertiesFunc only has attached
    property data for QQuickMaterialStyle (i.e. attached property types
    created from C++). We work around this by finding the first C++
    meta object, which works even for attached types created in QML.
*/
const QMetaObject *QQuickAttachedPropertyPropagatorPrivate::firstCppMetaObject(
    QQuickAttachedPropertyPropagator *propagator)
{
    return QQmlData::ensurePropertyCache(propagator)->firstCppMetaObject();
}

/*!
    \internal

    Tries to find a QQuickAttachedPropertyPropagator whose type is \a ourAttachedType
    and is attached to an ancestor of \a attachee.

    QQuickAttachedPropertyPropagator needs to know who its parent attached object is in
    order to inherit attached property values from it. This is called when an
    instance of QQuickAttachedPropertyPropagator is created, and whenever
    \c {attachee}'s parent changes, for example.
*/
QQuickAttachedPropertyPropagator *QQuickAttachedPropertyPropagatorPrivate::findAttachedParent(
    const QMetaObject *attachedType, QObject *attachee)
{
    qCDebug(lcAttachedPropertyPropagator).noquote() << "findAttachedParent called with"
        << attachedType->className() << attachee;

    Q_ASSERT(attachee);

    // A QQmlEngine is the fallback; it can't have an attached parent.
    if (qobject_cast<QQmlEngine *>(attachee))
        return {};

    QObject *object = attachee;
    while (object) {
        qCDebug(lcAttachedPropertyPropagator).noquote()
            << "- checking for attached parent on object" << object;

        if (auto *attachedTarget = qobject_cast<QQuickAttachedPropertyPropagatorTarget *>(object)) {
            qCDebug(lcAttachedPropertyPropagator)
                << "- object is a QQuickAttachedPropertyPropagatorTarget; delegating to attachedParent()";
            // If attachedParent returns nullptr, we assume we should continue the search.
            if (auto *objectAttached = attachedTarget->attachedParent(attachedType)) {
                qCDebug(lcAttachedPropertyPropagator).noquote().nospace()
                    << "- target has attached object " << objectAttached << "; returning";
                return objectAttached;
            }

            // QQuickPopup::attachedParent already checks the popup's window, so the next step is
            // to check the engine fallback.
            object = nullptr;
        } else if (auto *window = qobject_cast<QQuickWindow *>(object)) {
            // It doesn't seem like a parent window can be anything but transient in Qt Quick.
            QQuickWindow *parentWindow = qobject_cast<QQuickWindow *>(window->transientParent());
            qCDebug(lcAttachedPropertyPropagator).noquote()
                << "- object is a window; checking its parent window:" << parentWindow;
            if (parentWindow) {
                QQuickAttachedPropertyPropagator *attached
                    = QQuickAttachedPropertyPropagator::attachedObject(attachedType, parentWindow);
                if (attached) {
                    qCDebug(lcAttachedPropertyPropagator).noquote().nospace()
                        << "- parent window has attached object " << attached << "; returning";
                    return attached;
                }

                // Try the parent window's parent (TODO: will this actually ever get hit?)
                object = parentWindow->transientParent();
            } else {
                qCDebug(lcAttachedPropertyPropagator).noquote() << "- reached root window; ending search";
                object = nullptr;
            }
        } else if (auto *item = qobject_cast<QQuickItem *>(object)) {
            qCDebug(lcAttachedPropertyPropagator).noquote()
                << "- object is an item; calling attachedPropertyPropagator_parent() on it";
            // If this returns nullptr, we assume we should continue the search.
            if (auto *objectAttached = QQuickItemPrivate::get(item)->attachedPropertyPropagator_parent(attachedType))
                return objectAttached;

            object = item->parentItem();

            // If we ran out of parent items, try our window. Note that we can't just set object to
            // item->window() here and let the next loop iteration do the check, because the window
            // check in the loop checks the window's transient parent, so it would skip the window
            // itself.
            if (!object) {
                QQuickAttachedPropertyPropagator *attached
                    = QQuickAttachedPropertyPropagator::attachedObject(attachedType, item->window());
                if (attached) {
                    qCDebug(lcAttachedPropertyPropagator).noquote()
                    << "- parent window has attached object" << attached << "- returning";
                    return attached;
                }

                // Now try the window's transientParent(), via the next loop iteration.
                object = item->window();
            }
        } else {
            // We don't know what to do with this.
            object = nullptr;
        }
    } while (object);

    // We found no attached parent: use the engine as a fallback.
    if (auto *fallbackAttached = QQuickAttachedPropertyPropagator::fallbackAttachedObject(
            attachedType, attachee)) {
        return fallbackAttached;
    }

    return {};
}

QList<QQuickAttachedPropertyPropagator *> QQuickAttachedPropertyPropagatorPrivate::findAttachedChildren(
    const QMetaObject *attachedType, QObject *object)
{
    qCDebug(lcAttachedPropertyPropagator).noquote() << "findAttachedChildren called with"
        << attachedType->className() << object;

    QList<QQuickAttachedPropertyPropagator *> children;

    QQuickItem *item = qobject_cast<QQuickItem *>(object);
    if (!item) {
        QQuickWindow *window = qobject_cast<QQuickWindow *>(object);
        if (window)
            item = window->contentItem();
    }

    if (!item) {
        qCDebug(lcAttachedPropertyPropagator).noquote() << "- object is not an item; returning early";
        return children;
    }

    // At this point, "item" could either be an item that the attached object is
    // attached to directly, or the contentItem of a window that the attached object
    // is attached to.

    // Look for attached properties on items.
    const auto childItems = item->childItems();
    for (QQuickItem *child : childItems) {
        QQuickAttachedPropertyPropagator *attached
            = QQuickAttachedPropertyPropagator::attachedObject(attachedType, child);
        qCDebug(lcAttachedPropertyPropagator).noquote() << "  - checking child" << child;
        if (attached) {
            qCDebug(lcAttachedPropertyPropagator).noquote().nospace() << "  - child " << child
                << " has attached object " << attached << "; adding it to the list of children";
            children += attached;
        } else {
            qCDebug(lcAttachedPropertyPropagator).noquote() << "  - child" << child
                << "has no attached object; calling findAttachedChildren...";
            const QList<QQuickAttachedPropertyPropagator *> attachedChildren
                = findAttachedChildren(attachedType, child);
            children += attachedChildren;
            qCDebug(lcAttachedPropertyPropagator).noquote() << "  - ... found and added"
                << attachedChildren.size() << "children of" << child;
        }
    }

    // Look for attached properties on windows. Windows declared in QML
    // as children of a Window are QObject-parented to the contentItem (see
    // QQuickWindowPrivate::data_append()). Windows declared as children
    // of items are QObject-parented to those items.
    const auto &windowChildren = item->children();
    for (QObject *child : windowChildren) {
        QQuickWindow *childWindow = qobject_cast<QQuickWindow *>(child);
        if (childWindow) {
            QQuickAttachedPropertyPropagator *attached
                = QQuickAttachedPropertyPropagator::attachedObject(attachedType, childWindow);
            if (attached)
                children += attached;
        }
    }

    return children;
}

QQuickItem *QQuickAttachedPropertyPropagatorPrivate::findAttachedItem(QObject *object)
{
    QQuickItem *item = qobject_cast<QQuickItem *>(object);
    if (item)
        return item;

    if (auto *attachedTarget = qobject_cast<QQuickAttachedPropertyPropagatorTarget *>(object))
        return attachedTarget->attacheeItem();

    return {};
}

QQuickAttachedPropertyPropagator::QQuickAttachedPropertyPropagator(QObject *parent)
    : QObject(*(new QQuickAttachedPropertyPropagatorPrivate), parent)
{
    d_func()->init();
}

QQuickAttachedPropertyPropagator::QQuickAttachedPropertyPropagator(QQuickAttachedPropertyPropagatorPrivate &dd, QObject *parent)
    : QObject(dd, parent)
{
    d_func()->init();
}

QQuickAttachedPropertyPropagator::~QQuickAttachedPropertyPropagator()
{
    Q_D(QQuickAttachedPropertyPropagator);
    d->detachFrom(parent());
    d->setAttachedParent(nullptr);
}

QList<QQuickAttachedPropertyPropagator *> QQuickAttachedPropertyPropagator::attachedChildren() const
{
    Q_D(const QQuickAttachedPropertyPropagator);
    return d->attachedChildren;
}

QQuickAttachedPropertyPropagator *QQuickAttachedPropertyPropagator::attachedParent() const
{
    Q_D(const QQuickAttachedPropertyPropagator);
    return d->attachedParent;
}

QQuickAttachedPropertyPropagator *QQuickAttachedPropertyPropagator::attachedObject(
    const QMetaObject *attachedType, QObject *attachee, bool create)
{
    if (!attachee)
        return nullptr;
    auto func = qmlAttachedPropertiesFunction(attachee, attachedType);
    return qobject_cast<QQuickAttachedPropertyPropagator *>(qmlAttachedPropertiesObject(attachee, func, create));
}

/*!
    \internal

    Returns the fallback attached object for the given \a attachedType and \a attachee.

    The \l QQmlEngine associated with \a attachee is used as the fallback
    object. The fallback object is used to ensure that \a attachee has an
    attached parent if none can be found during \l initialize().
*/
QQuickAttachedPropertyPropagator *QQuickAttachedPropertyPropagator::fallbackAttachedObject(
    const QMetaObject *attachedType, QObject *attachee)
{
    QQmlEngine *engine = qmlEngine(attachee);
    qCDebug(lcAttachedPropertyPropagator).noquote() << "- falling back to engine" << engine;
    if (engine) {
        QByteArray name = QByteArray("_q_") + attachedType->className();
        auto *attached = engine->property(name).value<QQuickAttachedPropertyPropagator *>();
        if (!attached) {
            attached = QQuickAttachedPropertyPropagator::attachedObject(attachedType, engine, true);
            engine->setProperty(name, QVariant::fromValue(attached));
        }
        return attached;
    }
    return nullptr;
}

void QQuickAttachedPropertyPropagator::initialize()
{
    Q_D(QQuickAttachedPropertyPropagator);
    using Private = QQuickAttachedPropertyPropagatorPrivate;
    auto ownMetaObject = Private::firstCppMetaObject(this);
    if (auto type = QQmlMetaType::firstQmlTypeForAttachmentMetaObject(ownMetaObject); type.isValid()) {
        d->attacherMetaObject = type.metaObject();
    }

    qCDebug(lcAttachedPropertyPropagator) << "initialize called for" << parent()
        << "- looking for attached parent...";
    QQuickAttachedPropertyPropagator *attachedParent = Private::findAttachedParent(d->attacherMetaObject, parent());
    if (attachedParent)
        d->setAttachedParent(attachedParent);

    qCDebug(lcAttachedPropertyPropagator) << "- looking for attached children";
    const QList<QQuickAttachedPropertyPropagator *> attachedChildren
        = Private::findAttachedChildren(d->attacherMetaObject, parent());
    qCDebug(lcAttachedPropertyPropagator) << "- found" << attachedChildren.size()
        << "attached children of" << parent();
    for (QQuickAttachedPropertyPropagator *child : attachedChildren) {
        qCDebug(lcAttachedPropertyPropagator) << "  -" << child->parent();
        QQuickAttachedPropertyPropagatorPrivate::get(child)->setAttachedParent(this);
    }

    qCDebug(lcAttachedPropertyPropagator) << "... finished initializing";
}

void QQuickAttachedPropertyPropagator::attachedParentChange(
    QQuickAttachedPropertyPropagator *newParent, QQuickAttachedPropertyPropagator *oldParent)
{
    Q_UNUSED(newParent);
    Q_UNUSED(oldParent);
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug debug, const QQuickAttachedPropertyPropagator *propagator)
{
    QDebugStateSaver saver(debug);
    debug.nospace().noquote();
    if (!propagator) {
        debug << "QQuickAttachedPropertyPropagator(nullptr)";
        return debug;
    }

    // Cast to QObject to avoid recursion.
    debug << static_cast<const QObject *>(propagator) << " (which is attached to " << propagator->parent() << ')';
    return debug;
}
#endif

} // namespace QtPrivate

QT_END_NAMESPACE

#include "moc_qquickattachedpropertypropagator_p.cpp"
