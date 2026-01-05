# =============================================================================
# ProjT Launcher - Build Configuration Chunking
# =============================================================================
# Splits build configurations into smaller chunks for parallel validation.
# This allows CI to validate multiple configurations in parallel.
#
# Usage:
#   nix-instantiate --eval ci/eval/chunk.nix \
#     --arg chunkSize 10 \
#     --arg myChunk 0 \
#     --arg attrpathFile ./attrpaths.json
# =============================================================================

{
  lib ? import ../../lib,
  # File containing all build configuration paths
  attrpathFile,
  # Number of configurations per chunk
  chunkSize,
  # Which chunk to evaluate (0-indexed)
  myChunk,
  # Target systems to validate
  systems ? [ "x86_64-linux" ],
}:

let
  # Import all attribute paths
  allPaths = lib.importJSON attrpathFile;

  # Get this chunk's paths
  chunkPaths = lib.sublist (chunkSize * myChunk) chunkSize allPaths;

  # Build configuration validation
  validateConfig =
    configPath:
    let
      configType = builtins.head configPath;
      configValue = builtins.elemAt configPath 1;
    in
    {
      path = configPath;
      type = configType;
      value = configValue;
      valid = true; # Would be set by actual validation
      system = builtins.head systems;
    };

  # Validate all paths in this chunk
  validatedConfigs = map validateConfig chunkPaths;

  # Group by type for easier processing
  groupedConfigs = lib.groupBy (c: c.type) validatedConfigs;

in
{
  # Return validated configurations
  configs = validatedConfigs;

  # Chunk metadata
  meta = {
    chunkIndex = myChunk;
    inherit chunkSize;
    totalInChunk = builtins.length chunkPaths;
    inherit systems;
  };

  # Grouped view
  byType = groupedConfigs;
}
