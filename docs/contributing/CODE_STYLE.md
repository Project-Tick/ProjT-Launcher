# 📝 Code Style and Formatting

## ⚠️ Zero Tolerance Policy

Code style violations will cause the CI pipeline to fail immediately. **No exceptions.**

## C++ Formatting (Mandatory)

**All C++ files must be formatted with `clang-format` before committing.**

```bash
# Single file
clang-format -i path/to/file.cpp

# All files (Windows PowerShell)
Get-ChildItem -Path "launcher" -Recurse -Include "*.cpp","*.h" | ForEach-Object { clang-format -i $_.FullName }
```

### Basic Format Rules (`.clang-format`)

```yaml
BasedOnStyle: Chromium
IndentWidth: 4
ColumnLimit: 140
Standard: c++20
UseTab: Never
PointerAlignment: Left
BreakBeforeBraces: Custom
```

## C++ Coding Standards

### 1. Modern C++ Usage

**Why?** Modern C++ (C++20) features prevent bugs that were common in older versions.

- **`auto`**: Use `auto` only when the type is obvious (e.g., `auto* widget = new QWidget();`) or for iterators.
  - *Why?* It makes code cleaner, but overusing it hides important type information.
  - **`nullptr`**: Always use `nullptr`, never `NULL` or `0`.
    - *Why?* `NULL` is just the number 0. `nullptr` is a specific type that prevents accidental math with pointers.
  - **`override`**: All virtual function overrides must be marked with `override`.
    - *Why?* If you change the parent function, the compiler will tell you if you forgot to update the child.
  - **`const`**: Methods that do not modify the object state **must** be marked `const`.
    - *Why?* It promises the compiler (and other programmers) that this function is safe to call without changing anything.

### 2. Memory Management

**Why?** Memory leaks (forgetting to delete memory) are the #1 cause of crashes in C++.

- **Raw Pointers (`*`)**: **Forbidden** for ownership.
  - *Bad:* `MyClass* obj = new MyClass();` (Who deletes this? Often forgotten.)
  - **Smart Pointers**: Use `std::unique_ptr` or `std::shared_ptr`.
    - *Good:* `auto obj = std::make_unique<MyClass>();` (Automatically deleted when it goes out of scope.)
  - **Qt Parent Ownership**: Raw pointers are allowed **only** if they are parented to a `QObject`.
    - *Good:* `new QButton(this);` (Qt deletes the button when `this` is deleted.)

### 3. Error Handling

**Why?** Crashes are bad. We want to handle errors gracefully.

- **Exceptions**: Avoid them. They are slow and hard to track.
- **`std::optional`**: Use when a value might be missing.
  - *Example:* `std::optional<User> findUser(id)` (Returns a User or nothing).
  - **`std::expected`**: Use when an operation can fail.
    - *Example:* Returns either the Result OR an Error.

### 4. Lambda Expressions

**Why?** Lambdas are small functions inside functions. They are powerful but dangerous if they use variables that no longer exist.

- **Capture**: Avoid default capture `[=]` or `[&]`.
  - *Why?* It's easy to accidentally capture a huge object or a pointer that becomes invalid.
  - **`this` Capture**: Be extremely careful capturing `this` in async operations.
    - *Why?* If the object is deleted before the async task finishes, the app crashes. Use `QPointer`.
  - **Forward Declarations**: `class MyClass;` instead of `#include "MyClass.h"`.
    - *Why?* If you only use a pointer, you don't need the full file. This makes builds faster.

### 5. Headers and Includes

**Why?** Proper organization speeds up compilation.

- **`#pragma once`**: Mandatory. Prevents the file from being read twice.
- **Forward Declarations**: `class MyClass;` instead of `#include "MyClass.h"`.
  - *Why?* If you only use a pointer, you don't need the full file. This makes builds faster.

### 6. Naming Conventions

**Why?** Consistency makes code readable. We use `m_` to instantly know if a variable belongs to the class.

| Type | Format | Example | Why? |
| ------ | -------- | --------- | ------ |
| Class/Struct | PascalCase | `ThemeViewModel` | Standard C++ style. |
| Private Member | `m_` + camelCase | `m_currentTheme` | **Crucial.** Distinguishes `m_name` (class variable) from `name` (local variable). |
| Static Member | `s_` + camelCase | `s_instance` | Tells you this variable is shared across all instances. |
| Public Member | camelCase | `dateOfBirth` | Standard for structs. |
| Constant | SCREAMING_SNAKE | `MAX_VALUE` | Screaming means "DO NOT CHANGE". |
| Function | camelCase | `getCurrentTheme()` | Qt style. |

### 7. Comments and Documentation

- **Public API**: Public methods should be commented if their behavior is not obvious.
- **Complex Logic**: Must be commented with `//` explaining *why*, not *what*.
- **TODOs**: Must include your username: `// TODO(username): Fix this`.

---

## QML Coding Standards

### 1. Formatting

- **Indentation**: 4 spaces. No tabs.
- **Property Order**: Strict ordering required.

```qml
Type {
    id: root // 1. ID (Mandatory first)

    // 2. Layout/Positioning
    anchors.fill: parent
    width: 100
    height: 100

    // 3. Visual Properties
    color: "red"
    visible: true

    // 4. Custom Properties
    property string title: ""

    // 5. Signals
    signal clicked()

    // 6. Functions
    function doSomething() {}

    // 7. Child Objects
    Text { ... }
}
```

### 2. Logic in QML

- **Limit**: No JavaScript blocks longer than **5 lines**.
- **Complex Logic**: Must be moved to a C++ ViewModel or a separate `.js` file.
- **`console.log`**: **Forbidden** in production code. Use C++ logging or a custom logger wrapper.

### 3. Component API Design

- **Root Element**: The root element of a component should usually be `Item` or `Rectangle`.
  - **Property Aliasing**: Expose internal properties using `property alias`.

```qml
// ✅ CORRECT
property alias text: label.text

// ❌ INCORRECT
property string text: ""
onTextChanged: label.text = text
```

- **Implicit Size**: Always define `implicitWidth` and `implicitHeight` for reusable components.

### 4. Theme Usage

- **Hardcoded Colors**: **Strictly Forbidden**. You must use `ThemeColors`.
- **Hardcoded Sizes**: **Strictly Forbidden**. You must use `Theme` constants (e.g., `Theme.spacingM`).

### 5. Performance

- **Bindings**: Avoid complex bindings that evaluate frequently.
- **Images**: Always use `sourceSize` when loading large images.
- **Loaders**: Use `Loader` for heavy components that are not immediately visible.
- **Anchors vs Layouts**: Prefer `anchors` for simple positioning (faster). Use `Layouts` (RowLayout, ColumnLayout) for complex, dynamic UIs.

### 6. Signal Handlers

- **Naming**: Explicitly defined signal handlers (e.g., `onClicked:`) are preferred over `Connections` for direct children.
- **Parameters**: Always specify parameters if used (e.g., `onClicked: (mouse) => { ... }`).

---

## 📋 Standard File Templates (Copy & Paste)

Use these templates when creating new files to ensure you follow all rules.

## 1. C++ Header Template (`.h`)

```cpp
// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
 *
 *  This file is part of ProjT Launcher and is licensed under
 *  the GNU General Public License version 3 or later.
 *
 *  If this file includes work from previous open-source projects,
 *  their original copyright and license notices are preserved below.
 */

#pragma once

#include <QObject>
#include <memory>

/// @brief Short description of what this class does.
///
/// Detailed description if necessary.
class MyNewClass : public QObject {
    Q_OBJECT

   public:
    explicit MyNewClass(QObject* parent = nullptr);
    ~MyNewClass() override;

    bool doSomething(const QString& input);

   signals:
    void somethingHappened();

   private:
    void internalHelper();

    QString m_data;
};
```

## 2. C++ Source Template (`.cpp`)

```cpp
// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
 *
 *  This file is part of ProjT Launcher and is licensed under
 *  the GNU General Public License version 3 or later.
 *
 *  If this file includes work from previous open-source projects,
 *  their original copyright and license notices are preserved below.
 */

#include "MyNewClass.h"

#include <QDebug>

MyNewClass::MyNewClass(QObject* parent) : QObject(parent) {
    // Initialization
}

MyNewClass::~MyNewClass() = default;

bool MyNewClass::doSomething(const QString& input) {
    if (input.isEmpty()) {
        return false;
    }
    
    m_data = input;
    emit somethingHappened();
    return true;
}

void MyNewClass::internalHelper() {
    // Private logic
}
```

## 3. QML Component Template (`.qml`)

```qml
// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
 *
 *  This file is part of ProjT Launcher and is licensed under
 *  the GNU General Public License version 3 or later.
 *
 *  If this file includes work from previous open-source projects,
 *  their original copyright and license notices are preserved below.
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import ProjTLauncher 1.0
import "components"

Item {
    id: root

    // Public API
    property string text: ""
    signal clicked()

    // Layout
    implicitWidth: 200
    implicitHeight: 50

    Rectangle {
        anchors.fill: parent
        color: ThemeColors.surface
        radius: 4 // Use Theme constants if available

        Text {
            anchors.centerIn: parent
            text: root.text
            color: ThemeColors.text
            font.pixelSize: 14
        }

        MouseArea {
            anchors.fill: parent
            onClicked: root.clicked()
        }
    }
}
```

## 4. CMakeLists.txt Template (New Module)

```cmake
# Define the library
add_library(ProjT_MyModule STATIC)

# Add sources
target_sources(ProjT_MyModule PRIVATE
    MyClass.cpp
    MyClass.h
)

# Link dependencies
target_link_libraries(ProjT_MyModule PRIVATE
    Qt6::Core
    Qt6::Network
    ProjT_Utils
)

# Include directories
target_include_directories(ProjT_MyModule PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
```
