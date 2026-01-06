# =============================================================================
# ProjT Launcher - CI Utility Functions
# =============================================================================
# Helper functions for build configuration analysis and comparison.
# =============================================================================

{ lib, ... }:

rec {
  # Get unique strings from a list
  uniqueStrings = list: builtins.attrNames (builtins.groupBy lib.id list);

  # =============================================================================
  # File Path Analysis
  # =============================================================================

  # Get the category of a file based on its path
  getFileCategory =
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
    else if lib.hasPrefix "translations/" filePath then
      "translations"
    else if lib.hasPrefix "docs/" filePath then
      "documentation"
    else if lib.hasPrefix ".github/" filePath then
      "ci"
    else if lib.hasPrefix "ci/" filePath then
      "ci"
    else
      "other";

  # Get platform from file path if applicable
  getPlatformFromPath =
    filePath:
    if lib.hasInfix "linux" filePath then
      "linux"
    else if lib.hasInfix "darwin" filePath || lib.hasInfix "macos" filePath then
      "macos"
    else if lib.hasInfix "windows" filePath || lib.hasInfix "win32" filePath then
      "windows"
    else
      null;

  # =============================================================================
  # Change Classification
  # =============================================================================

  # Classify changed files by category
  classifyChanges =
    changedFiles:
    let
      categorized = map (f: {
        file = f;
        category = getFileCategory f;
        platform = getPlatformFromPath f;
      }) changedFiles;
    in
    lib.groupBy (c: c.category) categorized;

  # =============================================================================
  # Build Impact Analysis
  # =============================================================================

  # Determine if a file change requires rebuild
  requiresRebuild =
    filePath:
    let
      category = getFileCategory filePath;
    in
    builtins.elem category [
      "core"
      "ui"
      "minecraft"
      "modplatform"
      "java"
      "networking"
      "libraries"
      "build"
    ];

  # Get list of files that require rebuild
  getFilesRequiringRebuild = changedFiles: builtins.filter requiresRebuild changedFiles;

  # =============================================================================
  # Platform Analysis
  # =============================================================================

  # Group changes by affected platform
  groupByPlatform =
    changes:
    let
      platformChanges = map (c: {
        inherit (c) file category;
        platform = c.platform or "all";
      }) changes;
    in
    lib.groupBy (c: c.platform) platformChanges;

  # =============================================================================
  # Label Generation
  # =============================================================================

  # Generate labels based on changes
  getLabels =
    classifiedChanges:
    let
      categories = builtins.attrNames classifiedChanges;
      categoryLabels = map (cat: "category:${cat}") categories;

      rebuildCount = builtins.length (
        builtins.filter (c: requiresRebuild c.file) (lib.flatten (builtins.attrValues classifiedChanges))
      );

      rebuildLabels =
        if rebuildCount == 0 then
          [ ]
        else if rebuildCount <= 10 then
          [ "rebuild:small" ]
        else if rebuildCount <= 50 then
          [ "rebuild:medium" ]
        else
          [ "rebuild:large" ];
    in
    lib.listToAttrs (
      map (l: {
        name = l;
        value = true;
      }) (categoryLabels ++ rebuildLabels)
    );

  # =============================================================================
  # Component Analysis
  # =============================================================================

  # Extract affected components from file paths
  extractComponents =
    changedFiles:
    let
      components = map (
        f:
        let
          parts = lib.splitString "/" f;
        in
        if builtins.length parts >= 2 then builtins.elemAt parts 1 else null
      ) changedFiles;
    in
    uniqueStrings (builtins.filter (c: c != null) components);

  # Check if core functionality is affected
  isCoreAffected =
    changedFiles:
    builtins.any (
      f:
      lib.hasPrefix "launcher/Application" f
      || lib.hasPrefix "launcher/BaseInstance" f
      || lib.hasPrefix "launcher/FileSystem" f
    ) changedFiles;

  # =============================================================================
  # Summary Generation
  # =============================================================================

  # Generate change summary
  generateSummary =
    changedFiles:
    let
      classified = classifyChanges changedFiles;
      components = extractComponents changedFiles;
      rebuildsNeeded = getFilesRequiringRebuild changedFiles;
    in
    {
      totalFiles = builtins.length changedFiles;
      categories = builtins.attrNames classified;
      categoryCount = lib.mapAttrs (_: v: builtins.length v) classified;
      inherit components;
      rebuildRequired = builtins.length rebuildsNeeded > 0;
      rebuildCount = builtins.length rebuildsNeeded;
      coreAffected = isCoreAffected changedFiles;
      labels = getLabels classified;
    };
}
