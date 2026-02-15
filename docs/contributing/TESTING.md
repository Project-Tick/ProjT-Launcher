# Testing

Testing standards and practices.

**Framework**: QtTest is the only supported test framework. Do not introduce other frameworks.

---

## Requirements

| Change Type | Test Required |
|-------------|---------------|
| New logic | Yes |
| Bug fix | Yes (regression) |
| Refactor | Recommended |
| UI layout | No |
| Docs | No |

---

## Test Structure

```cpp
#include <QTest>
#include "MyClass.h"

class MyClassTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();    // Before all tests
    void cleanupTestCase(); // After all tests
    void init();            // Before each test
    void cleanup();         // After each test

    void test_methodName_condition();
};

QTEST_GUILESS_MAIN(MyClassTest)
#include "MyClassTest.moc"
```

---

## Naming

- **File**: `ClassName_test.cpp`
- **Class**: `ClassNameTest`
- **Method**: `test_method_condition`

---

## Async Testing

Use `QSignalSpy` instead of `sleep()`:

```cpp
QSignalSpy spy(obj, &MyClass::finished);
obj->startAsync();
QVERIFY(spy.wait(1000));
QCOMPARE(spy.count(), 1);
```

---

## Mocking

Create fake implementations for external dependencies:

```cpp
class FakeNetworkService : public INetworkService {
public:
    void get(const QString& url) override {
        emit finished("Fake Data");
    }
};
```

**Forbidden practices**:

- Real network requests
- Real file system access outside `QTemporaryDir`
- User accounts or credentials
- `sleep()` or timing-based waits (use `QSignalSpy::wait()`)
- System-specific state dependencies

## UI Testing

UI tests are generally discouraged. Prefer testing core logic via unit tests.

If UI testing is necessary:

- Keep scope minimal (widget behavior only)
- Do not test layout or visual appearance
- Use `QTest::mouseClick()` and `QTest::keyClick()` for interaction
- Isolate widgets from services using mocks

---

## Performance

- Single test: < 100ms
- Mark slow tests clearly
- Use `QTemporaryDir` for file tests

---

## Running Tests

**VS Code**: Testing tab → Run

**Command line (with presets)**:

```bash
ctest --preset default --output-on-failure
```

**Without presets**:

```bash
cd build
ctest --output-on-failure
```

**Single test**:

```bash
./build/<preset>/tests/MyClassTest
```

---

## CI

- All tests run via `ctest`
- Failing tests block merge
- Must pass on all platforms

## Launcher Coverage

- UI is excluded from core coverage requirements
- See the launcher subsystem test matrix: [Launcher Test Matrix](./LAUNCHER_TEST_MATRIX.md)

---

## Related

- [Architecture](./ARCHITECTURE.md)
- [Project Structure](./PROJECT_STRUCTURE.md)
