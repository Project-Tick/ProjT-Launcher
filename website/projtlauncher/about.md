---
layout: layouts/main.njk
title: About
templateClass: tmpl-post
tags: nav
hero:
  title: About ProjT Launcher
  description: "The custom launcher for Minecraft, built for the community."
  button:
    content: "Download"
    url: "/projtlauncher/download/"
eleventyNavigation:
  key: About
  order: 3
---
<div class="content">

# What is ProjT Launcher?

ProjT Launcher is a custom launcher for Minecraft that was forked from MultiMC. It allows the creation, and management of multiple installations of the game, or "instances" rather, each having their own separate mods, resource packs, settings, and more. While originally being forked over packaging/redistribution concerns, ProjT Launcher brings new and exciting features developed by the community for the community. Furthermore, ProjT Launcher is still just as true to its roots as ever in providing a project that vows to never bring hostility to those looking to fork, redistribute, or repackage, so long as the terms of our license are respected.

## I'm in! Now tell me more about these features?

- Managing multiple, isolated instances of Minecraft.
- Easy installation of mod loaders, such as Forge, Fabric, LiteLoader and Quilt.
- Manage your worlds, resource packs, and shader packs in just a few clicks.
- Install modpacks from many platforms such as CurseForge, Technic, FTB, FTB Legacy and Modrinth.
- Install individual mods from CurseForge and Modrinth, automatically tailored to match your loader and game version.
- Kill Minecraft easily in the event of a crash or freeze.
- Access all your logs easily.
- Built using Qt technologies, allowing the launcher to be both very performant and light on system resources.
- And much, much more.

</div>
<div class="infobox top">

# Maintainers

<ul class="user-list">
  {% for user in developers.users %}
  <li class="user-info">
    <a href="{{user[2]}}">
      <img src="{{user[1]}}" alt="Avatar">
      <div class="user-info-overlay">
        <div class="user-name">{{user[0]}}</div>
      </div>
    </a>
  </li>
  {% endfor %}
</ul>

# Moderators

<ul class="user-list">
  {% for user in moderators.users %}
  <li class="user-info">
    <a href="{{user[2]}}">
      <img src="{{user[1]}}" alt="Avatar">
      <div class="user-info-overlay">
        <div class="user-name">{{user[0]}}</div>
      </div>
    </a>
  </li>
  {% endfor %}
</ul>

</div>
<div class="content top">

# Contributors

<ul class="user-list">
  {% for user in contributors.users %}
  <li class="user-info user-contrib">
    <a href="{{user[1]}}">
      <img src="{{user[0]}}" alt="Avatar">
    </a>
  </li>
  {% endfor %}
</ul>
</div>

<div class="content top" id="license">

# License

The **ProjT Launcher** core application is licensed under the **GNU General Public License v3.0** (GPL-3.0).

The **ProjT Launcher Website** (this repository) and related web assets are licensed under the **GNU Affero General Public License v3.0** (AGPL-3.0).

As a monorepo, this repository includes multiple projects and libraries which may be licensed differently. Please refer to the specific `LICENSE` files in the respective directories or the [repository root](https://github.com/Project-Tick/ProjT-Launcher) for full details.

</div>
