# =============================================================================
# ProjT Launcher - CODEOWNERS Validator
# =============================================================================
# Validates the OWNERS file to ensure proper maintainer assignments.
# This helps maintain accurate code ownership across the project.
#
# Usage:
#   nix-build ci/codeowners-validator
# =============================================================================

{
  buildGoModule,
  fetchFromGitHub,
  fetchpatch,
  lib,
}:

buildGoModule {
  pname = "codeowners-validator";
  version = "0.7.4-projt";

  src = fetchFromGitHub {
    owner = "mszostok";
    repo = "codeowners-validator";
    rev = "f3651e3810802a37bd965e6a9a7210728179d076";
    hash = "sha256-5aSmmRTsOuPcVLWfDF6EBz+6+/Qpbj66udAmi1CLmWQ=";
  };

  patches = [
    # Allow checking user write access
    (fetchpatch {
      name = "user-write-access-check";
      url = "https://github.com/mszostok/codeowners-validator/compare/f3651e3810802a37bd965e6a9a7210728179d076...840eeb88b4da92bda3e13c838f67f6540b9e8529.patch";
      hash = "sha256-t3Dtt8SP9nbO3gBrM0nRE7+G6N/ZIaczDyVHYAG/6mU=";
    })
    # Custom permissions patch for ProjT Launcher
    ./permissions.patch
    # Allow custom OWNERS file path via OWNERS_FILE env var
    ./owners-file-name.patch
  ];

  postPatch = "rm -r docs/investigation";

  vendorHash = "sha256-R+pW3xcfpkTRqfS2ETVOwG8PZr0iH5ewroiF7u8hcYI=";

  meta = {
    description = "CODEOWNERS validator for ProjT Launcher";
    homepage = "https://github.com/mszostok/codeowners-validator";
    license = lib.licenses.asl20;
    mainProgram = "codeowners-validator";
  };
}
