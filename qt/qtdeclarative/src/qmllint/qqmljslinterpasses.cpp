// Copyright (C) 2026 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0
// Qt-Security score:significant

#include "qqmljslinterpasses_p.h"
#include "qqmljsliteralbindingcheck_p.h"

#include <private/qqmlsa_p.h>
#include <private/qqmljsscope_p.h>

#include <QtCore/qstring.h>

QT_BEGIN_NAMESPACE

namespace QQmlJSLinterPasses {

using namespace Qt::StringLiterals;
void registerDefaultPasses(QQmlSA::PassManager *passMan)
{
    passMan->registerPropertyPass(std::make_unique<QQmlJSLiteralBindingCheck>(passMan), QString(),
                                  QString(), QString());

    QQmlSA::PropertyPassBuilder(passMan)
            .withOnCall([](QQmlSA::PropertyPass *self, const QQmlSA::Element &, const QString &,
                           const QQmlSA::Element &, QQmlSA::SourceLocation location) {
                self->emitWarning("Do not use 'eval'", qmlEval, location);
            })
            .registerOnBuiltin("GlobalObject", "eval");

    QQmlSA::PropertyPassBuilder(passMan)
            .withOnRead([](QQmlSA::PropertyPass *self, const QQmlSA::Element &element,
                           const QString &propName, const QQmlSA::Element &readScope_,
                           QQmlSA::SourceLocation location) {
                const auto &elementScope = QQmlJSScope::scope(element);
                const auto &owner = QQmlJSScope::ownerOfProperty(elementScope, propName).scope;
                if (!owner || owner->isComposite() || owner->isValueType())
                    return;
                const auto &prop = QQmlSA::PropertyPrivate::property(element.property(propName));
                if (prop.index() != -1 && !prop.isPropertyConstant() && prop.notify().isEmpty()
                    && prop.bindable().isEmpty()) {
                    const QQmlJSScope::ConstPtr &readScope = QQmlJSScope::scope(readScope_);
                    // FIXME: we currently get the closest QML Scope as readScope, instead of
                    // the innermost scope. We try locate it here via source location
                    Q_ASSERT(readScope->scopeType() == QQmlJSScope::ScopeType::QMLScope);
                    for (auto it = readScope->childScopesBegin(); it != readScope->childScopesEnd();
                         ++it) {
                        QQmlJS::SourceLocation childLocation = (*it)->sourceLocation();
                        if (childLocation.offset <= location.offset()
                            && (childLocation.offset + childLocation.length
                                <= location.offset() + location.length())) {
                            if ((*it)->scopeType() != QQmlSA::ScopeType::BindingFunctionScope)
                                return;
                        }
                    }
                    const QString msg =
                            "Reading non-constant and non-notifiable property %1. "_L1
                            "Binding might not update when the property changes."_L1.arg(propName);
                    self->emitWarning(msg, qmlStalePropertyRead, location);
                }
            })
            .registerOn({}, {}, {});
}
} // namespace QQmlJSLinterPasses
QT_END_NAMESPACE
