# 🏗 Architecture and Separation of Concerns

## ⚠️ UI/Logic Separation

The launcher UI uses **Qt Widgets** (`.ui` files + C++). Keep UI code in `launcher/ui/` and keep core logic in `launcher/`, `launcher/minecraft/`, `launcher/net/`, `launcher/modplatform/`, `launcher/java/`, and `launcher/tasks/`.

**Rule of thumb:** UI classes should **display state and forward user intent**. They should not perform long-running or stateful operations directly.

## Layers (Simple Model)

1. **UI Layer (Qt Widgets)**
   - Files: `launcher/ui/`
   - Responsibilities: rendering, user input, invoking actions
   - Avoid direct file I/O or network access here.

2. **Core/Domain Layer**
   - Files: `launcher/`, `launcher/minecraft/`, `launcher/net/`, `launcher/modplatform/`, `launcher/java/`
   - Responsibilities: data models, networking, settings, launch logic
   - Avoid UI dependencies in this layer.

3. **Task System**
   - Files: `launcher/tasks/`
   - Responsibilities: long-running or async work (downloads, extracting, indexing)
   - UI starts tasks and listens to signals for progress/completion.

## UI to Core Communication

Prefer signals/slots and existing service objects over direct calls from UI to deep internals. For long work, use `Task` and connect progress signals to UI elements.

```cpp
auto task = makeShared<MyTask>();
connect(task.get(), &Task::progress, this, &MainWindow::updateProgress);
connect(task.get(), &Task::failed, this, &MainWindow::showError);
task->start();
```

## Threading & Concurrency

- **Main thread**: UI rendering only.
- **Worker threads**: File I/O, networking, hashing, or launch preparation.
- Any operation taking more than a few milliseconds should be async (use `Task` or `QThread`).

## The Task System

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

```cpp
auto task = makeShared<MyTask>();
connect(task.get(), &Task::succeeded, this, &MyClass::onTaskDone);
task->start();
```

## Common Violations (Do Not Do This)

1. **Blocking the UI**: no `sleep()` or long loops in UI classes.
2. **UI in core**: avoid `QtWidgets` includes outside `launcher/ui/`.
3. **Direct I/O in UI**: move I/O into tasks or core services.

## Application Lifecycle (High-Level)

1. **Startup**: `main.cpp` initializes the `Application` singleton.
2. **Setup**: `Application` loads settings, accounts, and instances.
3. **UI Launch**: `Application::showMainWindow()` creates `MainWindow`.
4. **Shutdown**: state is saved and resources are released.
