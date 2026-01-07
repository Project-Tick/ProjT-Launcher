# ProjT Launcher Nix File Parser
# Validates all .nix files in the project for syntax errors

{
  lib,
  nix,
  runCommand,
}:
let
  nixFiles = lib.fileset.toSource {
    root = ../.;
    fileset = lib.fileset.fileFilter (file: file.hasExt "nix") ../.;
  };
in
runCommand "projt-nix-parse-${nix.name}"
  {
    nativeBuildInputs = [
      nix
    ];
  }
  ''
    export NIX_STORE_DIR=$TMPDIR/store
    export NIX_STATE_DIR=$TMPDIR/state
    nix-store --init

    cd "${nixFiles}"

    echo "Parsing Nix files in ProjT Launcher..."

    # Parse all .nix files to check for syntax errors
    find . -type f -iname '*.nix' | while read file; do
      echo "Checking: $file"
      nix-instantiate --parse "$file" >/dev/null
    done

    echo "All Nix files parsed successfully!"
    touch $out
  ''
