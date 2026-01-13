# =============================================================================
# ProjT Launcher - Maintainer Assignment Module
# =============================================================================
# Maps changed files to their maintainers based on OWNERS file.
# Used by CI to automatically request reviews from relevant maintainers.
# =============================================================================

{
  lib,
}:

{
  # List of changed file paths
  changedPaths ? [ ],
}:

let
  # =============================================================================
  # Maintainer Definitions
  # =============================================================================

  # Project maintainers (GitHub usernames)
  maintainers = {
    YongDo-Hyun = {
      github = "YongDo-Hyun";
      name = "YongDo Hyun";
      areas = [
        "core"
        "ui"
        "minecraft"
        "build"
        "ci"
        "all"
      ];
    };
    grxtor = {
      github = "grxtor";
      name = "GRXTOR";
      areas = [
        "core"
        "ui"
        "minecraft"
        "build"
        "ci"
        "all"
      ];
    };
  };

  # =============================================================================
  # File to Area Mapping
  # =============================================================================

  # Map file paths to areas of responsibility
  getArea =
    filePath:
    if lib.hasPrefix "launcher/ui/" filePath then
      "ui"
    else if lib.hasPrefix "launcher/qtquick/" filePath then
      "ui"
    else if lib.hasPrefix "launcher/minecraft/" filePath then
      "minecraft"
    else if lib.hasPrefix "launcher/modplatform/" filePath then
      "modplatform"
    else if lib.hasPrefix "launcher/java/" filePath then
      "java"
    else if lib.hasPrefix "launcher/net/" filePath then
      "networking"
    else if lib.hasPrefix "launcher/" filePath then
      "core"
    else if lib.hasPrefix "cmake/" filePath then
      "build"
    else if lib.hasSuffix "CMakeLists.txt" filePath then
      "build"
    else if lib.hasPrefix ".github/" filePath then
      "ci"
    else if lib.hasPrefix "ci/" filePath then
      "ci"
    else if lib.hasPrefix "translations/" filePath then
      "translations"
    else if lib.hasPrefix "docs/" filePath then
      "documentation"
    else
      "other";

  # =============================================================================
  # Maintainer Resolution
  # =============================================================================

  # Get maintainers for a specific area
  getMaintainersForArea =
    area:
    lib.filter (m: builtins.elem area m.areas || builtins.elem "all" m.areas) (
      builtins.attrValues maintainers
    );

  # Get maintainers for a file
  getMaintainersForFile = filePath: getMaintainersForArea (getArea filePath);

  # Get all affected maintainers for changed files
  getAffectedMaintainers =
    changedFiles:
    let
      allMaintainers = lib.concatMap getMaintainersForFile changedFiles;
      uniqueByGithub = lib.groupBy (m: m.github) allMaintainers;
    in
    lib.mapAttrsToList (_: ms: builtins.head ms) uniqueByGithub;

  # =============================================================================
  # Change Analysis
  # =============================================================================

  # Group changed files by area
  filesByArea = lib.groupBy getArea changedPaths;

  # Get affected areas
  affectedAreas = builtins.attrNames filesByArea;

  # Get maintainers who should be notified
  maintainersToNotify = getAffectedMaintainers changedPaths;

in
{
  # List of maintainer GitHub usernames to notify
  maintainers = map (m: m.github) maintainersToNotify;

  # Areas affected by changes
  areas = affectedAreas;

  # Detailed mapping of areas to files
  inherit filesByArea;

  # Full maintainer info
  maintainerDetails = maintainersToNotify;

  # Summary for CI output
  summary = {
    totalFiles = builtins.length changedPaths;
    inherit affectedAreas;
    maintainers = map (m: m.github) maintainersToNotify;
  };
}
