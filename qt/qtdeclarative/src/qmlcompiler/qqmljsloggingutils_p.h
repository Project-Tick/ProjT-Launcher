// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0
// Qt-Security score:significant

#ifndef QQMLJSLOGGINGUTILS_P_H
#define QQMLJSLOGGINGUTILS_P_H

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

#include <QtCore/qstring.h>
#include <qtqmlcompilerexports.h>

#include "qqmljsloggingutils.h"

QT_BEGIN_NAMESPACE

class QQmlToolingSettings;
class QCommandLineParser;

namespace QQmlJS {

using LoggerWarningId = QQmlSA::LoggerWarningId;
using WarningSeverity = QQmlSA::WarningSeverity;

class LoggerCategoryPrivate;

class Q_QMLCOMPILER_EXPORT LoggerCategory
{
    Q_DECLARE_PRIVATE(LoggerCategory)

public:
    enum Essentiality { Essential, NonEssential };

    LoggerCategory();
    LoggerCategory(
            const QString &name, const QString &settingsName, const QString &description,
            WarningSeverity severity, Essentiality essentiality = NonEssential);
    LoggerCategory(const LoggerCategory &);
    LoggerCategory(LoggerCategory &&) noexcept;
    LoggerCategory &operator=(const LoggerCategory &);
    LoggerCategory &operator=(LoggerCategory &&) noexcept;
    ~LoggerCategory();

    QString name() const;
    QString settingsName() const;
    QString description() const;
    WarningSeverity severity() const;
    bool isEssential() const;

    LoggerWarningId id() const;

    void setSeverity(WarningSeverity);

private:
    std::unique_ptr<QQmlJS::LoggerCategoryPrivate> d_ptr;
};

class Q_QMLCOMPILER_EXPORT LoggerCategoryPrivate
{
public:
    LoggerCategoryPrivate() = default;
    LoggerCategoryPrivate(const QString &name, const QString &settingsName,
                          const QString &description, WarningSeverity severity,
                          LoggerCategory::Essentiality essentiality);

    LoggerWarningId id() const { return LoggerWarningId(m_name); }

    QString name() const { return m_name; }
    QString settingsName() const { return m_settingsName; }
    QString description() const { return m_description; }
    bool isEssential() const { return m_isEssential; }

    WarningSeverity severity() const { return m_severity; }
    void setSeverity(WarningSeverity);

    bool hasChanged() const { return m_changed; }

    static LoggerCategoryPrivate *get(LoggerCategory *);

    friend bool operator==(const LoggerCategoryPrivate &lhs, const LoggerCategoryPrivate &rhs)
    {
        return operatorEqualsImpl(lhs, rhs);
    }
    friend bool operator!=(const LoggerCategoryPrivate &lhs, const LoggerCategoryPrivate &rhs)
    {
        return !operatorEqualsImpl(lhs, rhs);
    }

    bool operator==(const LoggerWarningId warningId) const { return warningId.name() == m_name; }

private:
    static bool operatorEqualsImpl(const LoggerCategoryPrivate &, const LoggerCategoryPrivate &);

    QString m_name;
    QString m_settingsName;
    QString m_description;
    WarningSeverity m_severity = WarningSeverity::Info;
    bool m_isEssential = false; // qmllint or other tooling might malfunction without it. Can't be disabled.
    bool m_changed = false;
};

namespace LoggingUtils {
enum class CategorySelection { All, Explicit };
Q_QMLCOMPILER_EXPORT void updateLogSeverities(
        QList<LoggerCategory> &categories, const QQmlToolingSettings &settings,
        QCommandLineParser *parser, CategorySelection categorySelection = CategorySelection::All);

Q_QMLCOMPILER_EXPORT std::optional<QQmlJS::WarningSeverity> severityFromString(const QString &);
Q_QMLCOMPILER_EXPORT QString severityToString(QQmlJS::WarningSeverity);
} // namespace LoggingUtils

} // namespace QQmlJS

QT_END_NAMESPACE

#endif // QQMLJSLOGGINGUTILS_P_H
