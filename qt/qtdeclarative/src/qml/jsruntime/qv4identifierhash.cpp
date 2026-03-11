// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant

#include <private/qv4identifierhash_p.h>
#include <private/qv4identifiertable_p.h>
#include <private/qv4string_p.h>
#include <private/qv4identifierhashdata_p.h>
#include <private/qprimefornumbits_p.h>

QT_BEGIN_NAMESPACE

namespace QV4 {

IdentifierHash::IdentifierHash(ExecutionEngine *engine)
{
    d = new IdentifierHashData(engine->identifierTable, 3);
    Q_ASSERT(isValid());
}

void IdentifierHash::addEntry(PropertyKey identifier, int value)
{
    Q_ASSERT(identifier.isStringOrSymbol());

    // fill up to max 50%
    bool grow = (d->alloc <= d->size*2);

    if (grow) {
        ++d->numBits;
        int newAlloc = qPrimeForNumBits(d->numBits);
        IdentifierHashEntry *newEntries = (IdentifierHashEntry *)malloc(newAlloc * sizeof(IdentifierHashEntry));
        memset(newEntries, 0, newAlloc*sizeof(IdentifierHashEntry));
        for (int i = 0; i < d->alloc; ++i) {
            const IdentifierHashEntry &e = d->entries[i];
            if (!e.identifier.isValid())
                continue;
            uint idx = e.identifier.id() % newAlloc;
            while (newEntries[idx].identifier.isValid()) {
                ++idx;
                idx %= newAlloc;
            }
            newEntries[idx] = e;
        }
        free(d->entries);
        d->entries = newEntries;
        d->alloc = newAlloc;
    }

    uint idx = identifier.id() % d->alloc;
    while (d->entries[idx].identifier.isValid()) {
        Q_ASSERT(d->entries[idx].identifier != identifier);
        ++idx;
        idx %= d->alloc;
    }
    d->entries[idx].identifier = identifier;
    ++d->size;
    (d->entries + idx)->value = value;
}

int IdentifierHash::lookup(PropertyKey identifier) const
{
    if (!d || !identifier.isStringOrSymbol())
        return -1;
    Q_ASSERT(d->entries);

    uint idx = identifier.id() % d->alloc;
    while (1) {
        if (!d->entries[idx].identifier.isValid())
            return -1;
        if (d->entries[idx].identifier == identifier)
            return (d->entries + idx)->value;
        ++idx;
        idx %= d->alloc;
    }
}

PropertyKey IdentifierHash::toIdentifier(const QString &str) const
{
    Q_ASSERT(d);
    return d->identifierTable->asPropertyKey(str, IdentifierTable::ForceConversionToId);
}

PropertyKey IdentifierHash::toIdentifier(Heap::String *str) const
{
    Q_ASSERT(d);
    return d->identifierTable->asPropertyKey(str);
}

PropertyKey IdentifierHash::reverseLookup(int value) const
{
    IdentifierHashEntry *e = d->entries;
    IdentifierHashEntry *end = e + d->alloc;
    while (e < end) {
        if (e->identifier.isValid() && e->value == value)
            return e->identifier;
        ++e;
    }
    return PropertyKey::invalid();
}

template<>
QString IdentifierHash::toString<QString>(PropertyKey key) const
{
    return key.isValid() ? key.toQString() : QString();
}

template<>
Heap::String *IdentifierHash::toString<Heap::String *>(PropertyKey key) const
{
    return key.isString() ? static_cast<Heap::String *>(key.asStringOrSymbol()) : nullptr;
}

QV4::IdentifierHash::IdentifierHash(const IdentifierHash &other)
{
    d = other.d;
    if (d)
        d->refCount.ref();
}

QV4::IdentifierHash::~IdentifierHash()
{
    if (d && !d->refCount.deref())
        delete d;
}

IdentifierHash &QV4::IdentifierHash::operator=(const IdentifierHash &other)
{
    if (other.d)
        other.d->refCount.ref();
    if (d && !d->refCount.deref())
        delete d;
    d = other.d;
    return *this;
}

int IdentifierHash::size() const
{
    return d ? d->size : 0;
}

} // namespace QV4

QT_END_NAMESPACE
