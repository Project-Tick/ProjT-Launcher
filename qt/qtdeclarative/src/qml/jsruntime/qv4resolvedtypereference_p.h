// Copyright (C) 2020 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant

#ifndef QV4RESOLVEDTYPEREFERNCE_P_H
#define QV4RESOLVEDTYPEREFERNCE_P_H

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

#include <QtQml/private/qtqmlglobal_p.h>
#include <QtQml/private/qqmlrefcount_p.h>
#include <QtQml/private/qqmlpropertycache_p.h>
#include <QtQml/private/qqmltype_p.h>
#include <QtQml/private/qv4compileddata_p.h>

QT_BEGIN_NAMESPACE

class QCryptographicHash;
namespace QV4 {

class ResolvedTypeReference
{
    Q_DISABLE_COPY_MOVE(ResolvedTypeReference)
public:
    ResolvedTypeReference() = default;

    QQmlPropertyCache::ConstPtr createPropertyCache();
    bool addToHash(QCryptographicHash *hash, QHash<quintptr, QByteArray> *checksums);

    void doDynamicTypeCheck();

    QQmlType type() const { return m_type; }
    void setType(QQmlType type)  {  m_type = std::move(type); }

    // The compilation unit is only stored for type references we need to create objects from.
    // For everything else the QQmlType and QQmlPropertyCache are enough.
    // Furthermore, if the reference points to a type from the same compilation unit, it also won't
    // hold a compilation unit since the object creator can just use the one it has already.
    QQmlRefPointer<QV4::CompiledData::CompilationUnit> compilationUnit()
    {
        Q_ASSERT(!m_isSelfReference);
        return m_compilationUnit;
    }

    void setCompilationUnit(const QQmlRefPointer<QV4::CompiledData::CompilationUnit> &unit)
    {
        Q_ASSERT(!m_isSelfReference);
        m_compilationUnit = unit;
    }

    QQmlPropertyCache::ConstPtr typePropertyCache() const { return m_typePropertyCache; }
    void setTypePropertyCache(QQmlPropertyCache::ConstPtr cache)
    {
        m_typePropertyCache = std::move(cache);
    }

    QTypeRevision version() const { return m_version; }
    void setVersion(QTypeRevision version) { m_version = version; }

    bool isFullyDynamicType() const { return m_isFullyDynamicType; }
    void setFullyDynamicType(bool fullyDynamic) { m_isFullyDynamicType = fullyDynamic; }

    // Whether the reference points to a type from the same compilation unit. This doesn't
    // have to be the top-level type but can also be an inline component.
    bool isSelfReference() const { return m_isSelfReference; }
    void setIsSelfReference(bool selfReference) { m_isSelfReference = selfReference; }

private:
    QQmlType m_type;
    QQmlPropertyCache::ConstPtr m_typePropertyCache;
    QQmlRefPointer<QV4::CompiledData::CompilationUnit> m_compilationUnit;

    QTypeRevision m_version = QTypeRevision::zero();
    // Types such as QQmlPropertyMap can add properties dynamically at run-time and
    // therefore cannot have a property cache installed when instantiated.
    bool m_isFullyDynamicType = false;
    bool m_isSelfReference = false;
};

} // namespace QV4

QT_END_NAMESPACE

#endif // QV4RESOLVEDTYPEREFERNCE_P_H
