// Copyright (C) 2023 The Qt Company Ltd.

#include <QtTest/qtest.h>

#include <QtCore/qbytearray.h>
#include <QtCore/qdir.h>
#include <QtCore/qfile.h>
#include <QtCore/qobject.h>
#include <QtCore/qstring.h>
#include <QtCore/qtemporaryfile.h>
#include <QtCore/qstandardpaths.h>

#if QT_CONFIG(process)
#include <QtCore/qprocess.h>
#endif

class tst_QmlCppCodegenVerify : public QObject
{
    Q_OBJECT
private slots:
    void verifyGeneratedSources_data();
    void verifyGeneratedSources();
};

void tst_QmlCppCodegenVerify::verifyGeneratedSources_data()
{
    QTest::addColumn<QString>("file");

    QDir a(":/a");
    const QStringList entries =  a.entryList(QDir::Files);
    for (const QString &entry : entries)
        QTest::addRow("%s", entry.toUtf8().constData()) << entry;
}

static QByteArray showDiff(const QByteArray &aData, const QByteArray &bData)
{
#if QT_CONFIG(process)
    QTemporaryFile aFile;
    if (!aFile.open())
        return "Error opening temp file a";
    aFile.write(aData);
    aFile.flush();

    QTemporaryFile bFile;
    if (!bFile.open())
        return "Error opening expected temp file";
    bFile.write(bData);
    bFile.flush();

    QProcess diffProc;
    QStringList arguments;
    if (auto git = QStandardPaths::findExecutable("git"); !git.isEmpty()) {
        diffProc.setProgram(git);
        arguments << "diff";
    } else {
        diffProc.setProgram("diff");
    }
    arguments << "-ub" << bFile.fileName() << aFile.fileName();
    diffProc.setArguments(std::move(arguments));

    diffProc.start();
    if (!diffProc.waitForStarted()) {
        return "Error waiting for " + diffProc.program().toLocal8Bit() + " process to start. ("
                + diffProc.errorString().toLocal8Bit() + ")";
    }
    if (!diffProc.waitForFinished()) {
        return "Error waiting for " + diffProc.program().toLocal8Bit() + " process to finish. ("
                + diffProc.errorString().toLocal8Bit() + ")";
    }

    return diffProc.readAllStandardOutput();
#else
    return "output differs";
#endif
}

void tst_QmlCppCodegenVerify::verifyGeneratedSources()
{
    QFETCH(QString, file);
    QFile a(":/a/" + file);
    QFile b(":/b/" + file.replace("codegen_test_module", "codegen_test_module_verify"));

    QVERIFY(a.open(QIODevice::ReadOnly));
    QVERIFY(b.open(QIODevice::ReadOnly));

    const QByteArray aData = a.readAll();
    const QByteArray bData = b.readAll()
                                     .replace("verify/TestTypes", "TestTypes")
                                     .replace("verify_TestTypes", "TestTypes");

    // Don't call the diff machinery if they are the same.
    if (aData != bData)
        QFAIL(showDiff(aData, bData));
}

QTEST_MAIN(tst_QmlCppCodegenVerify)

#include "tst_qmlcppcodegen_verify.moc"
