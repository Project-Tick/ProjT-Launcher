# Launcher Test Matrix (UI Excluded)

This document defines the **test coverage expectations** for all non-UI subsystems under `launcher/`.

## Scope

- **Included:** All core subsystems under `launcher/`
- **Excluded:** `launcher/ui/` (UI tests are tracked separately)

## Goals

- Each subsystem has at least **one unit/integration test**
- New or changed core behavior must be covered by tests
- All tests run via `ctest`
- Tests are platform-agnostic (Linux/Windows/macOS)

## Subsystem Coverage List

The table below shows the **target coverage**. “Covered” means baseline tests exist for the subsystem (UI excluded).

| Subsystem | Path(s) | Example Tests | Status |
|---|---|---|---|
| File system & archives | `launcher/FileSystem.*`, `launcher/GZip.*`, `launcher/MMCZip.*`, `launcher/Untar.*` | `FileSystem_test`, `GZip_test` | Covered |
| Versioning | `launcher/Version.*` | `Version_test` | Covered |
| String/Json helpers | `launcher/StringUtils.*`, `launcher/Json.*` | `StringUtils_test`, `Json_test` | Covered |
| SeparatorPrefixTree | `launcher/SeparatorPrefixTree.h` | `SeparatorPrefixTree_test` | Covered |
| INI/Config | `launcher/INIFile.*` | `INIFile_test` | Covered |
| Task system | `launcher/tasks/` | `Task_test` | Covered |
| Java management | `launcher/java/` | `JavaVersion_test`, `RuntimeVersion_test` | Covered |
| Minecraft pack parsing | `launcher/minecraft/mod/` | `DataPackParse_test`, `ResourcePackParse_test`, `TexturePackParse_test`, `ShaderPackParse_test`, `WorldSaveParse_test` | Covered |
| Minecraft version formats | `launcher/minecraft/` | `MojangVersionFormat_test`, `Version_test` | Covered |
| Library/Dependency | `launcher/minecraft/Library.*`, `launcher/GradleSpecifier.*` | `Library_test`, `GradleSpecifier_test` | Covered |
| Mod platforms | `launcher/modplatform/` | `ModPlatform_test` | Covered |
| Networking/Downloads | `launcher/net/` | `NetUtils_test`, `NetHeaderProxy_test`, `NetSink_test` | Covered |
| Launch pipeline | `launcher/launch/`, `launcher/LaunchController.*` | `LaunchVariableExpander_test`, `LaunchLogModel_test`, `LaunchLineRouter_test`, `LaunchPipeline_test` | Covered |
| Instance management | `launcher/Instance*.*`, `launcher/BaseInstance.*` | `InstanceCopyPrefs_test`, `BaseInstanceSettings_test` | Covered |
| Updater | `launcher/updater/` | `ProjTExternalUpdater_test` | Covered |
| Logs/Diagnostics | `launcher/logs/`, `launcher/XmlLogs.*` | `XmlLogs_test`, `LogEventParser_test`, `MessageLevel_test` | Covered |
| Meta parsing | `launcher/meta/` | `MetaComponentParse_test` | Covered |
| Resource model | `launcher/minecraft/ResourceFolderModel.*` | `ResourceFolderModel_test` | Covered |

## Rules For New Tests

- New subsystems must add a row here
- “Missing” or “Covered” areas require new tests
- UI tests are tracked separately

## CI Expectation

- All tests run via `ctest`
- CI runs the same test set on all platforms
