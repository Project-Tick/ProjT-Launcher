# Apple Silicon rationale and migration notes

This document contains the background, technical rationale, and alternatives related to the project's decision to target Apple Silicon (ARM64) for official builds and to deprecate active Intel macOS testing.

## Summary

As of 0.0.3 the project targets Apple Silicon (ARM64) for official builds and day-to-day testing. Intel macOS (x86_64) compatibility still exists in some artifacts (for example older releases or universal `.app` bundles built via CMake), but active Intel-specific testing and certain Intel build pipelines (notably the Nix flake builds) are deprecated.

This decision is driven by several practical factors affecting build reproducibility, CI availability, and long-term maintenance costs for a small and shrinking user base. It does not mean that universally packaged `.app` bundles cannot run on Intel macOS; it means we do not guarantee or actively test Intel-specific fixes across our primary build and CI workflows.

## Reasons (technical)

- Toolchain and platform support: modern macOS toolchains, SDKs, and some third-party libraries are increasingly ARM-first. Building and testing against the latest toolchains is simpler and faster on Apple Silicon.

- Reproducible/infrastructure builds: our Nix flake-based builds and other reproducible build paths are significantly slower and more brittle on x86_64-darwin. Maintaining separate derivations, self-hosted runners, or extensive workarounds increases maintenance burden.

- CI availability: many cloud CI providers now prioritize ARM macOS runners; securing reliable, up-to-date Intel macOS runners is costly or impractical.

- Maintenance cost vs. user share: supporting Intel macOS requires ongoing CI/packaging complexity and platform-specific debugging. For a small and declining user share this reduces overall project velocity.

## Observed issues in Intel builds (examples)

During testing we observed higher incidence of platform-specific problems on Intel macOS, including but not limited to:

- Increased crashes in Qt WebEngine-heavy paths
- Rendering differences or glitches that did not appear on Apple Silicon
- Increased resource usage and slower startup times in some scenarios

These are presented as observations from our internal testing; specific projects or environments may have different results.

## Alternatives for Intel Mac users

If you are on an Intel Mac and need to run ProjT Launcher, options include:

1. Use the last Intel-compatible release (for example v0.0.2).
2. Run a Linux x86_64 build in a VM or container.
3. Use a community-maintained launcher or fork that continues Intel support.

## Notes for packagers and distributors

- Our `.app` bundles built via CMake may be universal, but we do not actively test those on Intel hardware.
- If you package for a distribution that requires Intel macOS support you may need to maintain separate build pipelines and perform independent testing.

## Contact

If you represent a user population that requires continued Intel macOS support (for example a distribution or institutional environment), please open a GitHub issue describing the scope and constraints — we can discuss options (sponsored CI, community-maintained runners, or targeted backporting).
