# ProjT Launcher Flatpak Repository

This is the repository used to publish nightly builds of the [launcher](https://projecttick.org) to our own Flatpak remote at `https://flatpak.projecttick.org`.

## Usage

Our repository can be added through its `.flatpakrepo` in most software centers using [this link](https://flatpak.projecttick.org/projtlauncher.flatpakrepo). A `nightly` branch of ProjT Launcher should then appear in search!

You can also start using it through the CLI:

```bash
$ flatpak remote-add --if-not-exists projtlauncher https://flatpak.projecttick.org/projtlauncher.flatpakrepo
$ flatpak install org.projecttick.ProjTLauncher//nightly
```

### Installing with the Stable Release

Due to its isolation of apps by default, Flatpak has the nice advantage of allowing you to use the stable and nightly releases of ProjT at the same time!

To switch between the two, you can use the `flatpak make-current` command:

```bash
$ # Replace 'nightly' with 'stable' to switch back to the regular release
$ flatpak make-current org.projecttick.ProjTLauncher nightly
```

You can also specify which version to run with the `--branch` argument to `flatpak run`:

```bash
$ flatpak run --branch=nightly org.projecttick.ProjTLauncher
```

## Special Thanks

- [Flatpak's official documentation](https://docs.flatpak.org/en/latest/hosting-a-repository.html)
- [proletarius101's](https://gitlab.com/proletarius101) [GitLab CI template](https://gitlab.com/accessable-net/gitlab-ci-templates)
