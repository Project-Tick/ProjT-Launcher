# 🧪 Testing Standards

## ⚠️ Testing Policy

Tests are **mandatory** for most changes.  
PRs that introduce behavior changes without appropriate tests **may be rejected**.

**All new features must include reasonable test coverage.**

### When are tests required?

| Change Type | Test Required? |
| ----------- | ---------------- |
| New core logic (non-UI) | ✅ Yes |
| New task | ✅ Yes |
| Bug fix | ✅ Yes (regression if possible) |
| Refactor (no behavior change) | ⚠️ Recommended |
| UI-only layout change (.ui) | ❌ No |
| Docs / comments | ❌ No |

### Why do we write tests?

1. **Confidence**: You can change code without fear of breaking old features.
2. **Documentation**: Tests show exactly how your code is supposed to be used.
3. **Speed**: It's faster to run a test (10ms) than to launch the app and click buttons (30s).

- **New Features**: Should have reasonable test coverage.
- **Bug Fixes**: Should include a regression test if possible.
- **Flaky Tests**: Should be fixed or disabled.

## Test Framework

We use **QtTest** for all C++ testing. The tests are located in the `tests/` directory and are managed by CMake.

## Required Test Structure

```cpp
#include <QTest>
#include <QSignalSpy>
#include "MyClass.h"

class MyClassTest : public QObject {
    Q_OBJECT

   private slots:
    void initTestCase();    // Run once before all tests (Setup Database)
    void cleanupTestCase(); // Run once after all tests (Delete Database)
    void init();            // Run before each test (Reset variables)
    void cleanup();         // Run after each test (Free memory)

    // Naming: test_WhatWeAreTesting_WhatWeExpect
    void test_ShouldCalculateCorrectly_WhenInputIsValid();
    void test_ShouldThrowError_WhenInputIsInvalid();
};

// Use GUILESS for non-UI tests (Faster)
QTEST_GUILESS_MAIN(MyClassTest)
#include "MyClassTest.moc"
```

## Strict Rules

Tests that do not follow naming conventions will not be discovered by CI.

### 1. Naming Conventions

- **File**: `ClassName_test.cpp` (e.g., `FileSystem_test.cpp`)
- **Class**: `ClassNameTest`
- **Method**: `test_Condition_ExpectedResult` or `test_methodName_scenario`

### 2. Async Testing

- **Forbidden**: `QThread::sleep()`, `QCoreApplication::processEvents()` (unless absolutely necessary).
- **Mandatory**: Use `QSignalSpy` for signal verification.
- Any test relying on timing instead of signals will be rejected.

```cpp
// ✅ CORRECT
QSignalSpy spy(object, &MyClass::finished);
object->startAsyncOperation();
// Wait up to 1s. Returns true if signal fired.
QVERIFY(spy.wait(1000)); 
QCOMPARE(spy.count(), 1);
```

### 3. Mocking (Faking it)

**What is it?** Creating a fake version of a complex object.
**Why?** We don't want to actually download files from the internet during a test.

- Unit tests must not require internet access, user accounts, or system-specific state.

- **Do not** make real network requests in unit tests.
- Create "Fake" implementations of interfaces for testing.

```cpp
// The Real Service downloads from the internet
// The Fake Service just returns a string immediately
class FakeNetworkService : public INetworkService {
public:
    void get(const QString& url) override {
        emit finished("Fake Data");
    }
};
```

### 4. Performance

- A single test case should typically complete in under 100ms.
- Slow tests must be clearly marked and justified in the test name or comments.
- Heavy integration tests must be marked and separated.

## CI Enforcement

- All tests are run in CI using `ctest`.
- Failing tests will block merging.
- PRs that reduce test coverage without justification may be rejected.
- Tests must pass on all supported platforms.

## Running Tests

### Using VS Code

1. Open the **Testing** tab (Beaker icon).
2. Click the **Run** button next to the test you want to run.

### Using Command Line

```bash
cd build
ctest --verbose
```

## Integration Tests

Integration tests verify that multiple components work together.

- Place them in `tests/` alongside unit tests, but name them clearly.
- Use `QTemporaryDir` to create isolated file environments.

```cpp
void test_FullInstanceCreationFlow() {
    QTemporaryDir tempDir;
    InstanceService service(tempDir.path());
    // ...
}
```

- **Prefer**: Testing core logic in C++ (models, tasks, parsers).
- **UI tests**: Optional and should be minimal; keep them focused on widget behavior.

### ⚠️ Integration Tests vs Unit Tests

Integration tests must not replace unit tests.
Core logic must still be tested in isolation.

## Running Tests from CLI

```bash
# Run all tests
ctest --output-on-failure

# Run specific test
./tests/MyClassTest

# Run with Valgrind (Linux)
ctest -T memcheck
```
