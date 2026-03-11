// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant
#ifndef QV4IDENTIFIERHASH_P_H
#define QV4IDENTIFIERHASH_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <private/qv4global_p.h>
#include <private/qv4propertykey_p.h>

#include <QtCore/qstring.h>

QT_BEGIN_NAMESPACE

namespace QV4 {

struct IdentifierHashEntry;
struct IdentifierHashData;
struct IdentifierHash
{
    IdentifierHash() = default;
    IdentifierHash(ExecutionEngine *engine);
    IdentifierHash(const IdentifierHash &other);
    IdentifierHash(IdentifierHash &&other) noexcept : d(std::exchange(other.d, nullptr)) {}
    ~IdentifierHash();
    IdentifierHash &operator=(const IdentifierHash &other);

    void swap(IdentifierHash &other) noexcept { qt_ptr_swap(d, other.d); }
    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_MOVE_AND_SWAP(IdentifierHash)

    bool isValid() const { return d; }
    int size() const;

    void add(const QString &str, int value) { addEntry(toIdentifier(str), value); }
    void add(Heap::String *str, int value) { addEntry(toIdentifier(str), value); }

    int value(const QString &str) const { return lookup(toIdentifier(str)); }
    int value(Heap::String *str) const { return lookup(toIdentifier(str)); }

    template<typename String>
    String key(int value) const { return toString<String>(reverseLookup(value)); }

private:
    void addEntry(PropertyKey i, int value);

    int lookup(PropertyKey identifier) const;
    PropertyKey reverseLookup(int value) const;

    PropertyKey toIdentifier(const QString &str) const;
    PropertyKey toIdentifier(Heap::String *str) const;

    template<typename String>
    String toString(PropertyKey key) const;

    IdentifierHashData *d = nullptr;
};

template<> Heap::String *IdentifierHash::toString<Heap::String *>(PropertyKey key) const;
template<> QString IdentifierHash::toString<QString>(PropertyKey key) const;

} // namespace QV4

QT_END_NAMESPACE

#endif // QV4_IDENTIFIERHASH_P_H
