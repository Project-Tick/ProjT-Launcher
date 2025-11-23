#include <QDebug>
#include <QTest>

#include <minecraft/MojangVersionFormat.h>

class MojangVersionFormatTest : public QObject {
    Q_OBJECT

    static QJsonDocument readJson(const QString path)
    {
        QFile jsonFile(path);
        if (!jsonFile.open(QIODevice::ReadOnly)) {
            qWarning() << "Failed to open file '" << jsonFile.fileName() << "' for reading!";
            return QJsonDocument();
        }
        auto data = jsonFile.readAll();
        jsonFile.close();
        return QJsonDocument::fromJson(data);
    }
    static void writeJson(const char* file, QJsonDocument doc)
    {
        QFile jsonFile(file);
        if (!jsonFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qCritical() << "Failed to open file '" << jsonFile.fileName() << "' for writing!";
            return;
        }
        auto data = doc.toJson(QJsonDocument::Indented);
        qDebug() << QString::fromUtf8(data);
        jsonFile.write(data);
        jsonFile.close();
    }

   private slots:
    void test_Through_Simple()
    {
        QJsonDocument doc = readJson(QFINDTESTDATA("testdata/MojangVersionFormat/1.9-simple.json"));
        auto vfile = MojangVersionFormat::versionFileFromJson(doc, "1.9-simple.json");
        auto doc2 = MojangVersionFormat::versionFileToJson(vfile);
        writeJson("1.9-simple-passthorugh.json", doc2);

        QCOMPARE(doc.toJson(), doc2.toJson());
    }

    void test_Through()
    {
        QJsonDocument doc = readJson(QFINDTESTDATA("testdata/MojangVersionFormat/1.9.json"));
        auto vfile = MojangVersionFormat::versionFileFromJson(doc, "1.9.json");
        auto doc2 = MojangVersionFormat::versionFileToJson(vfile);
        writeJson("1.9-passthorugh.json", doc2);
        QCOMPARE(doc.toJson(), doc2.toJson());
    }

    void test_PlatformValidation()
    {
        // Create a library with various platforms in natives
        QJsonObject libObj;
        libObj["name"] = "test:test:1.0";
        
        QJsonObject nativesObj;
        nativesObj["linux"] = "natives-linux.jar";      // Should be accepted
        nativesObj["windows"] = "natives-windows.jar";  // Should be accepted
        nativesObj["osx"] = "natives-osx.jar";          // Should be accepted
        nativesObj["macos"] = "natives-macos.jar";      // Should be accepted (alternative name)
        nativesObj["freebsd"] = "natives-freebsd.jar";  // Should be accepted
        nativesObj["unknown-platform"] = "natives-unknown.jar";  // Should be skipped
        nativesObj["unsupported-os"] = "natives-unsupported.jar"; // Should be skipped
        
        libObj["natives"] = nativesObj;
        
        auto lib = MojangVersionFormat::libraryFromJson(libObj, "test.json");
        
        // Should have all supported platforms
        QVERIFY(lib->m_nativeClassifiers.contains("linux"));
        QVERIFY(lib->m_nativeClassifiers.contains("windows"));
        QVERIFY(lib->m_nativeClassifiers.contains("osx"));
        QVERIFY(lib->m_nativeClassifiers.contains("macos"));
        QVERIFY(lib->m_nativeClassifiers.contains("freebsd"));
        
        // Should NOT have unknown platforms
        QVERIFY(!lib->m_nativeClassifiers.contains("unknown-platform"));
        QVERIFY(!lib->m_nativeClassifiers.contains("unsupported-os"));
    }
};

QTEST_GUILESS_MAIN(MojangVersionFormatTest)

#include "MojangVersionFormat_test.moc"
