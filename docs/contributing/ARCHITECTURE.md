# Architecture

High-level design and component interaction in ProjT Launcher.

---

## Layers

```
┌─────────────────────────────────────────┐
│              UI Layer                   │
│         launcher/ui/ (Qt Widgets)       │
├─────────────────────────────────────────┤
│            Core Layer                   │
│   launcher/minecraft/, net/, java/      │
├─────────────────────────────────────────┤
│            Task System                  │
│           launcher/tasks/               │
└─────────────────────────────────────────┘
```

### UI Layer

**Location**: `launcher/ui/`

- Qt Widgets interface
- Renders state, handles user input
- No direct I/O or network access

### Core Layer

**Location**: `launcher/`, `launcher/minecraft/`, `launcher/net/`, `launcher/java/`

- Business logic
- Data models
- Network operations
- No UI dependencies

### Task System

**Location**: `launcher/tasks/`

- Long-running operations
- Async work (downloads, extraction)
- Progress reporting

---

## Key Principles

### Separation of Concerns

UI classes display state and forward user intent. They do not perform:

- File I/O
- Network requests
- Long computations

### Communication Pattern

UI communicates with core via signals/slots:

```cpp
auto task = makeShared<DownloadTask>(url);
connect(task.get(), &Task::progress, this, &MainWindow::updateProgress);
connect(task.get(), &Task::finished, this, &MainWindow::onDownloadComplete);
task->start();
```

### Threading

| Thread | Purpose |
|--------|---------|
| Main | UI rendering only |
| Worker | File I/O, networking, hashing |

Operations > 10ms should be async.

---

## Task System

### Creating a Task

```cpp
class MyTask : public Task {
    Q_OBJECT

protected:
    void executeTask() override {
        setStatus("Working...");
        setProgress(0);

        // Do work
        for (int i = 0; i < 100; i++) {
            if (isCancelled()) {
                emitFailed("Cancelled");
                return;
            }
            setProgress(i);
        }

        emitSucceeded();
    }
};
```

### Running a Task

```cpp
auto task = makeShared<MyTask>();
connect(task.get(), &Task::succeeded, this, &MyClass::onSuccess);
connect(task.get(), &Task::failed, this, &MyClass::onFailure);
task->start();
```

---

## Application Lifecycle

1. **Startup**: `main.cpp` creates `Application` singleton
2. **Setup**: Load settings, accounts, instances
3. **UI Launch**: Create `MainWindow`
4. **Runtime**: Event loop processes user actions
5. **Shutdown**: Save state, release resources

---

## Service Objects

Long-lived non-UI classes owned by Application:

- `AccountManager` - Microsoft/offline accounts
- `InstanceManager` - Minecraft instances
- `NetworkManager` - HTTP operations
- `SettingsManager` - Configuration

Access via `Application::instance()->serviceName()`.

---

## Common Violations

**Don't do these**:

| Violation | Fix |
|-----------|-----|
| `sleep()` in UI | Use Task |
| Network in UI class | Move to core service |
| UI import in core | Remove dependency |
| Direct file I/O in UI | Use Task |

---

## Module Dependencies

```
ui/ ──→ minecraft/ ──→ net/
         │              │
         └──→ tasks/ ←──┘
               │
               └──→ java/
```

**Rules**:

- No circular dependencies
- `ui/` depends on everything
- Core modules are independent
- `tasks/` is a utility layer

---

## Related

- [Project Structure](./PROJECT_STRUCTURE.md)
- [Testing](./TESTING.md)
