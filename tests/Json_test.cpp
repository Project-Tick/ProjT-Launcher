#include <QDebug>
#include <QTest>

#include <Json.h>

class JsonTest : public QObject {
    Q_OBJECT

   private slots:
    void test_PathTraversalProtection()
    {
        // Test that absolute paths are rejected
        QVERIFY_EXCEPTION_THROWN(Json::requireIsType<QDir>(QJsonValue("../../../etc/passwd"), "test"), Json::JsonException);

        // Test that relative paths work
        QDir dir = Json::requireIsType<QDir>(QJsonValue("mods"), "test");
        QVERIFY(dir.path().endsWith("mods"));

        // Test that dangerous characters are sanitized
        dir = Json::requireIsType<QDir>(QJsonValue("mods<>:\"|?*"), "test");
        QVERIFY(!dir.path().contains('<'));
        QVERIFY(!dir.path().contains('>'));
        QVERIFY(!dir.path().contains(':'));
        QVERIFY(!dir.path().contains('"'));
        QVERIFY(!dir.path().contains('|'));
        QVERIFY(!dir.path().contains('?'));
        QVERIFY(!dir.path().contains('*'));

        // Test that .. is removed
        dir = Json::requireIsType<QDir>(QJsonValue("../mods"), "test");
        QVERIFY(!dir.path().contains(".."));
    }
};

QTEST_GUILESS_MAIN(JsonTest)

#include "Json_test.moc"