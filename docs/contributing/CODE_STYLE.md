# Code Style

Formatting and coding standards for ProjT Launcher.

---

## Formatting

All C++ code must be formatted with clang-format before committing.

```sh
clang-format -i path/to/file.cpp
```

CI will reject unformatted code.

---

## C++ Standards

### Modern C++

| Feature | Usage |
|---------|-------|
| `auto` | Only when type is obvious |
| `nullptr` | Always (never `NULL` or `0`) |
| `override` | Required on all overrides |
| `const` | Required for non-mutating methods |

### Memory Management

**Smart Pointers**:

```cpp
// Good
auto obj = std::make_unique<MyClass>();

// Bad - raw pointer ownership
MyClass* obj = new MyClass();
```

**Qt Parent Ownership**:

```cpp
// Good - Qt manages lifetime
new QButton(this);
```

### Error Handling

- Avoid exceptions
- Use `std::optional` for missing values
- Use `std::expected` for operations that can fail

### Lambdas

```cpp
// Good - explicit capture
connect(btn, &QPushButton::clicked, this, [this, id]() {
    handleClick(id);
});

// Bad - default capture
connect(btn, &QPushButton::clicked, [=]() { ... });
```

---

## Naming Conventions

| Type | Format | Example |
|------|--------|---------|
| Class | PascalCase | `MainWindow` |
| Private member | `m_` + camelCase | `m_currentTheme` |
| Static member | `s_` + camelCase | `s_instance` |
| Public member | camelCase | `dateOfBirth` |
| Constant | SCREAMING_SNAKE | `MAX_VALUE` |
| Function | camelCase | `getCurrentTheme()` || Enum class | PascalCase | `enum class State` |
| Enum values | PascalCase | `State::Running` |

**SCREAMING_SNAKE scope**: Use for `constexpr` constants, `#define` macros, and legacy `enum` values only. Prefer `enum class` with PascalCase values for new code.
---

## Headers

### Include Guards

```cpp
#pragma once
```

### Include Order

1. Corresponding header
2. C++ standard library
3. Qt headers
4. Third-party headers
5. Project headers

```cpp
#include "MyClass.h"

#include <memory>
#include <string>

#include <QObject>
#include <QString>

#include "OtherClass.h"
```

**Note**: clang-format automatically reorders includes. Always accept its output.

### Forward Declarations

Prefer forward declarations over includes when possible:

```cpp
// Good
class OtherClass;

// Avoid
#include "OtherClass.h"
```

**Qt warning**: Forward declarations do not work for QObject-derived classes that use `Q_OBJECT`. These must be fully included.

---

## Comments

### Documentation

```cpp
/// @brief Short description.
///
/// Detailed description if needed.
/// @param input Description of parameter.
/// @return Description of return value.
bool doSomething(const QString& input);
```

### Implementation Comments

```cpp
// Why this approach was chosen
// NOT what the code does
```

### TODOs

```cpp
// TODO(username): Description of what needs to be done
// FIXME(username): Description of broken behavior that needs fixing
```

- **TODO**: Planned improvement or missing feature
- **FIXME**: Known bug or broken behavior requiring attention
- Username is required for traceability
- Compatible with IDE TODO/FIXME highlighting tools

---

## Qt Widgets

### UI Files

- Use Qt Designer for layout
- Never edit generated `ui_*.h` files
- Set meaningful `objectName` values

### Widget Classes

- Keep UI logic minimal
- Delegate to core services
- Use signals/slots for communication

---

## File Templates

These templates apply only to new Project Tick–owned files. Do not modify license headers in upstream forked code.

### Header (.h)

```cpp
// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 Project Tick

#pragma once

#include <QObject>

class MyClass : public QObject {
    Q_OBJECT

public:
    explicit MyClass(QObject* parent = nullptr);
    ~MyClass() override;

signals:
    void somethingHappened();

private:
    QString m_data;
};
```

### Source (.cpp)

```cpp
// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 Project Tick

#include "MyClass.h"

MyClass::MyClass(QObject* parent)
    : QObject(parent)
{
}

MyClass::~MyClass() = default;
```

---

## Related

- [Project Structure](./PROJECT_STRUCTURE.md)
- [Architecture](./ARCHITECTURE.md)
