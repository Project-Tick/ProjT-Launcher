// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef DECLARATIVELYREGISTERED_H
#define DECLARATIVELYREGISTERED_H

#include <QtCore/qobject.h>
#include <QtQml/qqmlregistration.h>

class PurelyDeclarativeSingleton : public QObject
{
    Q_OBJECT
    QML_SINGLETON
    QML_ELEMENT
public:
    PurelyDeclarativeSingleton(QObject *parent = nullptr);
};

class UncreatableDeclarativeSingleton : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(ProvidedSingleton)
    QML_SINGLETON
    QML_UNCREATABLE("provided by C++")
public:
    UncreatableDeclarativeSingleton(int foo, QObject *parent = nullptr);
private:
    int m_foo;
};

class CppObject: public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;
};

struct ForeignUncreatableSingleton
{
    Q_GADGET
    QML_FOREIGN(CppObject)
    QML_NAMED_ELEMENT(ForeignSingleton)
    QML_SINGLETON
    QML_UNCREATABLE("provided by C++")
};

#endif
