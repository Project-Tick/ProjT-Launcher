// Copyright (C) 2026 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0
// Qt-Security score:significant

#ifndef QQMLJSLINTERPASSES_P_H
#define QQMLJSLINTERPASSES_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#include <QtQmlCompiler/qqmlsa.h>

QT_BEGIN_NAMESPACE

namespace QQmlJSLinterPasses {
void registerDefaultPasses(QQmlSA::PassManager *passManager);
}

QT_END_NAMESPACE

#endif // QQMLJSLINTERPASSES_P_H
