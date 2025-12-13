# 🏗 Architecture and MVVM

## ⚠️ MVVM Compliance

We follow the **MVVM (Model-View-ViewModel)** architecture. Business logic should be kept out of QML files to ensure testability and maintainability.

## The Pattern (MVVM Explained Simply)

We use **Model-View-ViewModel (MVVM)**. If you are new to this, think of a **Thermostat**:

1. **Model (The Sensor)**:
    * It knows the *actual* temperature (e.g., 20.5°C).
    * It doesn't care if anyone is looking at it.
    * *In our code:* `MinecraftInstance`, `Account`, `Settings`.

2. **ViewModel (The Computer)**:
    * It reads the sensor.
    * It decides if the heater should be ON.
    * It formats "20.5" to "20°C" for the display.
    * *In our code:* `InstanceViewModel`, `LoginViewModel`.

3. **View (The Screen)**:
    * It just shows "20°C".
    * If you click "Up", it tells the ViewModel "User wants warmer".
    * *In our code:* `HomePage.qml`, `LoginDialog.qml`.

### 1. View (QML)

* **Role**: Display data and capture user input.
* **Strict Rule**: **NO BUSINESS LOGIC**.
  * *Why?* We cannot write Unit Tests for QML easily. Logic must be in C++ to be tested.
  * ✅ `onClicked: viewModel.submitForm()`
  * ❌ `onClicked: { if (password.length > 5) { ... } }` (Move validation to C++)
* **Strict Rule**: **NO DIRECT DATABASE/FILE ACCESS**.
  * *Why?* The UI should not know how files are stored.

### 2. ViewModel (C++)

* **Role**: Expose state and commands to the View.
* **Strict Rule**: **NO UI DEPENDENCIES**.
  * *Why?* We might want to change the UI later (e.g., to a command line interface). The logic shouldn't break.
  * ❌ `#include <QtWidgets>` (Forbidden)
  * ❌ `QMessageBox::show()` (Forbidden - use a signal to request a dialog)
* **Strict Rule**: All properties **MUST** have a `NOTIFY` signal.
  * *Why?* If the data changes in C++, the UI needs to know immediately to update the screen.

### 3. Model (C++)

* **Role**: Data storage, networking, file I/O.
* **Strict Rule**: **NO QML DEPENDENCIES**.
  * *Why?* The data layer should be pure C++.

### 4. The Bridge (Connecting C++ to QML)

The connection between C++ ViewModels and QML Views happens in `launcher/ui/QmlMainWindow.cpp`.

* **Mechanism**: We use `QQmlContext::setContextProperty` to make C++ objects available in QML.
* **Naming Convention**: ViewModels are exposed with camelCase names (e.g., `launcherViewModel`, `settingsViewModel`).

```cpp
// Example from QmlMainWindow.cpp
void QmlMainWindow::exposeContextProperties(...) {
    // This makes 'launcherViewModel' available in all QML files
    context->setContextProperty("launcherViewModel", launcherViewModel);
    context->setContextProperty("settingsViewModel", settingsViewModel);
}
```

## Implementation Rules

### 1. Property Definitions

Every `Q_PROPERTY` must follow this pattern:

```cpp
// ✅ CORRECT
// READ: The function to get the value
// WRITE: The function to set the value
// NOTIFY: The signal fired when value changes
Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

// ❌ INCORRECT (Missing NOTIFY)
// The UI won't update if you change this variable!
Q_PROPERTY(QString name READ name WRITE setName)
```

### 2. Threading & Concurrency

* **Main Thread**: UI rendering only.
* **Worker Threads**: File I/O, Networking, Hashing, Game Launching.
* **Strict Rule**: Any operation taking >10ms must be async.
  * *Why?* If you block the main thread, the window freezes and says "Not Responding".
* **Mechanism**: Use `Task` system or `QThread`.

### 3. Dependency Injection

* ViewModels should not instantiate their dependencies directly if possible.
* Use the `Application` singleton to access global services (e.g., `Application::instance()->settings()`).

## The Task System

For long-running operations (downloading, extracting, launching), we use a `Task` system.

### Creating a Task

Inherit from `Task` and implement `executeTask()`.

```cpp
class MyTask : public Task {
    Q_OBJECT
protected:
    void executeTask() override {
        setStatus("Doing something...");
        setProgress(0);
        
        // Do work...
        if (failed) {
            emitFailed("Something went wrong");
            return;
        }
        
        emitSucceeded();
    }
};
```

### Running a Task

Tasks are run by the `TaskRunner` or directly if they are simple.

```cpp
MyTask* task = new MyTask();
connect(task, &Task::succeeded, this, &MyClass::onTaskDone);
task->start();
```

## Common Violations (Do Not Do This)

1. **Calling QML from C++**: Never use `findChild<QObject*>()` to manipulate QML objects from C++.
2. **God Classes**: Do not dump everything into `Application.cpp`. Create specific services.
3. **Blocking the UI**: Never use `QThread::sleep()` or blocking `waitForFinished()` on the main thread.

## Application Lifecycle

1. **Startup**: `main.cpp` initializes `Application` singleton.
2. **Setup**: `Application` loads settings, accounts, and instances.
3. **UI Launch**: `QmlMainWindow` is created and shows `ShellRoot.qml`.
4. **Shutdown**: `Application` saves state and cleans up resources.
        text: qsTr("Apply Theme")
        onClicked: {
            if (themeVM) {
                themeVM.applyTheme()
            }
        }
    }
}

```qml

---

# 🎨 Theme and Color System

## ThemeColors.qml Singleton

```qml
// Usage
import ProjTLauncher 1.0

Rectangle {
    color: ThemeColors.background
    border.color: ThemeColors.border
    
    Text {
        color: ThemeColors.text
        font.pointSize: ThemeColors.fontBody
    }
}
```

## Available Color Constants

|Constant|Description|
|`ThemeColors.background`|Main background|
|`ThemeColors.backgroundAlt`|Alternative background|
|`ThemeColors.surface`|Card/panel surface|
|`ThemeColors.text`|Main text color|
|`ThemeColors.textSecondary`|Secondary text|
|`ThemeColors.border`|Border color|
|`ThemeColors.accent`|Accent color|
|`ThemeColors.primary`|Primary color|
|`ThemeColors.highlight`|Selection highlight|
|`ThemeColors.toolBar`|Toolbar background|
|`ThemeColors.error`|Error color|
|`ThemeColors.success`|Success color|
|`ThemeColors.warning`|Warning color|

## Theme.js Constants

```javascript
// Spacing
Theme.spacingXS  // 4px
Theme.spacingS   // 8px
Theme.spacingM   // 12px
Theme.spacingL   // 16px

// Radius
Theme.radiusS    // 4px
Theme.radiusM    // 8px
Theme.radiusL    // 12px

// Font sizes
Theme.fontCaption   // 11pt
Theme.fontBody      // 12pt
Theme.fontSubtitle  // 14pt
Theme.fontHeader    // 18pt
```
