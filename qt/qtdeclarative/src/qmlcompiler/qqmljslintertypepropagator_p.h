// Copyright (C) 2026 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0
// Qt-Security score:significant

#ifndef QQMLJSLINTERTYPEPROPAGATOR_P_H
#define QQMLJSLINTERTYPEPROPAGATOR_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#include "qqmljstypepropagator_p.h"

QT_BEGIN_NAMESPACE

struct ContextPropertyInfo
{
    QQmlJS::HeuristicContextProperties heuristicContextProperties;
    QQmlJS::UserContextProperties userContextProperties;
};


class Q_QMLCOMPILER_EXPORT QQmlJSLinterTypePropagator : public QQmlJSTypePropagator
{
public:
    QQmlJSLinterTypePropagator(const QV4::Compiler::JSUnitGenerator *unitGenerator,
                               const QQmlJSTypeResolver *typeResolver, QQmlJSLogger *logger,
                               const BasicBlocks &basicBlocks = {},
                               const InstructionAnnotations &annotations = {},
                               QQmlSA::PassManager *passManager = nullptr,
                               const ContextPropertyInfo &contextPropertyInfo = {});

private:

    void generate_Ret() override;
    void generate_LoadQmlContextPropertyLookup(int index) override;
    void generate_GetOptionalLookup(int index, int offset) override;
    void generate_StoreProperty(int nameIndex, int base) override;
    void generate_CallProperty(int nameIndex, int base, int argc, int argv) override;
    void generate_CallPossiblyDirectEval(int argc, int argv) override;

    void handleUnqualifiedAccess(const QString &name, bool isMethod) const override;
    void handleUnqualifiedAccessAndContextProperties(const QString &name, bool isMethod) const override;
    void checkDeprecated(QQmlJSScope::ConstPtr scope, const QString &name, bool isMethod) const override;
    bool isCallingProperty(QQmlJSScope::ConstPtr scope, const QString &name) const override;
    bool handleImportNamespaceLookup(const QString &propertyName) override;
    void handleLookupError(const QString &propertyName) override;
    bool checkForEnumProblems(QQmlJSRegisterContent base, const QString &propertyName) override;
    void generate_StoreNameCommon(int nameIndex) override;
    void propagatePropertyLookup(const QString &name,
                                 int lookupIndex = QQmlJSRegisterContent::InvalidLookupIndex) override;
    void propagateCall(const QList<QQmlJSMetaMethod> &methods, int argc, int argv,
                       QQmlJSRegisterContent scope) override;
    void propagateTranslationMethod_SAcheck(const QString &methodName) override;
    void warnAboutTypeCoercion(int lhs) override;

    QQmlSA::PassManager *m_passManager = nullptr;
    ContextPropertyInfo m_contextPropertyInfo;
};

QT_END_NAMESPACE

#endif // QQMLJSLINTERTYPEPROPAGATOR_P_H
