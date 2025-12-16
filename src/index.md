---
layout: layouts/main.njk
title: Home
hero:
  title: 'ProjT Launcher'
  description: 'An open source Minecraft launcher with the ability to manage multiple instances, accounts and
  mods. Focused on user freedom and free redistributability.'
  image:
    light: '/img/screenshots/projtlauncher_windows_dark_main_window.png'
    dark: '/img/screenshots/projtlauncher_windows_dark_main_window.png'
  button:
    url: /download?from=button
    content: 'Download Now'
---

<div class="home-sections">
  <section class="section feature-section">
    <div class="section-heading">
      <p class="eyebrow">Built for control</p>
      <h2>Stay in charge of every Minecraft instance</h2>
      <p>ProjT Launcher keeps your modded adventures isolated, reproducible, and easy to maintain. Install, update, and launch everything from lightweight vanilla installs to massive modpacks with a single tool.</p>
    </div>
    <div class="feature-grid">
      <article class="feature-card">
        <div class="feature-icon">01</div>
        <h3>Modpack comfort</h3>
        <p>Install curated modpacks from Modrinth or CurseForge in seconds, keep them clean, and roll back when the pack maintainer ships a surprise update.</p>
      </article>
      <article class="feature-card">
        <div class="feature-icon">02</div>
        <h3>Smart updates</h3>
        <p>Track individual mods, version changes, and dependencies so you always know what changed before clicking download.</p>
      </article>
      <article class="feature-card">
        <div class="feature-icon">03</div>
        <h3>Isolated instances</h3>
        <p>Each instance has its own folder, assets, and configuration. Jump back in time without overwriting worlds or losing settings.</p>
      </article>
      <article class="feature-card">
        <div class="feature-icon">04</div>
        <h3>Themeable &amp; lightweight</h3>
        <p>ProjT Launcher uses the Qt toolkit and supports custom themes, so it stays snappy on low-spec machines and still feels personal.</p>
      </article>
    </div>
  </section>

  <section class="section spotlight-section">
    <div class="spotlight-grid">
      <article class="spotlight-card">
        <h3>Modpack management without the mess</h3>
        <p>Switch modloaders, tweak JVM arguments, or fork configuration profiles for friends. Everything lives inside a portable folder that you can back up or sync.</p>
        <ul>
          <li>Automatic downloads from Modrinth &amp; CurseForge</li>
          <li>Version pinning and downgrade support</li>
          <li>Log console with real-time progress</li>
        </ul>
        <img src="/img/screenshots/projtlauncher_windows_dark_install_modpacks_modrinth.png" alt="ProjT Launcher installing a Modrinth pack">
      </article>
      <article class="spotlight-card">
        <h3>Granular control over every mod</h3>
        <p>Missing a single client mod? Search, download, and update it inside the launcher. No more digging through folders or re-downloading entire packs.</p>
        <ul>
          <li>Curated discovery surface for the mods you need</li>
          <li>Per-instance overrides keep experiments contained</li>
          <li>Update previews show what will change before you apply</li>
        </ul>
        <img src="/img/screenshots/projtlauncher_windows_dark_install_mods.png" alt="Managing mods directly in ProjT Launcher">
      </article>
    </div>
  </section>

  <section class="section workflow-section">
    <div class="section-heading">
      <p class="eyebrow">Launcher workflow</p>
      <h2>From download to launch in four calm steps</h2>
      <p>You shouldn't have to fight your launcher. ProjT keeps each phase focused and predictable so you can move from idea to gameplay without fires to put out.</p>
    </div>
    <div class="workflow-grid">
      <article class="workflow-step">
        <div class="workflow-index">01</div>
        <h3>Pick a platform</h3>
        <p>Grab the installer that fits your OS. All binaries are signed and mirrored, so getting started is as easy on Linux as it is on Windows.</p>
      </article>
      <article class="workflow-step">
        <div class="workflow-index">02</div>
        <h3>Create instances</h3>
        <p>Spin up lightweight vanilla profiles or giant modded setups. Each instance remembers its mods, saves, and JVM state independently.</p>
      </article>
      <article class="workflow-step">
        <div class="workflow-index">03</div>
        <h3>Sync &amp; customize</h3>
        <p>Connect Modrinth and CurseForge accounts, apply your favorite theme, and wire up automatic backups before you hop in.</p>
      </article>
      <article class="workflow-step">
        <div class="workflow-index">04</div>
        <h3>Launch with confidence</h3>
        <p>Per-instance logs, resource tracking, and built-in updates mean you always know what changed before you smash play.</p>
      </article>
    </div>
  </section>

  <section class="section ecosystem-section">
    <div class="section-heading">
      <p class="eyebrow">Ecosystem ready</p>
      <h2>Bring your favorite services with you</h2>
    </div>
    <div class="integration-grid">
      <article class="integration-card">
        <div class="integration-icon"><i class="fa fa-cubes" aria-hidden="true"></i></div>
        <h3>Modrinth &amp; CurseForge</h3>
        <p>Browse, install, and update packs from both modding giants without opening a browser.</p>
        <ul>
          <li>Side-by-side changelog previews</li>
          <li>Optional dependency suggestions</li>
          <li>Fast mirror-aware downloads</li>
        </ul>
      </article>
      <article class="integration-card">
        <div class="integration-icon"><i class="fa fa-cloud" aria-hidden="true"></i></div>
        <h3>Catpacks &amp; community repos</h3>
        <p>Ship your own curated experiences with Catpacks or point instances at any open repository.</p>
        <ul>
          <li>Deterministic manifests per pack</li>
          <li>Shareable URLs for your community</li>
          <li>Override support for power users</li>
        </ul>
      </article>
      <article class="integration-card">
        <div class="integration-icon"><i class="fa fa-comments" aria-hidden="true"></i></div>
        <h3>Matrix-powered support</h3>
        <p>Matrix rooms stay close to the launcher so you can get help, contribute, or celebrate a release in one tap.</p>
        <ul>
          <li>Directly linked knowledge base</li>
          <li>Friendly moderation team</li>
          <li>Office-hours for contributors</li>
        </ul>
      </article>
    </div>
  </section>

  <section class="section stats-section">
    <div class="section-heading">
      <p class="eyebrow">Why players switch</p>
      <h2>Freedom-focused from the first line of code</h2>
      <p>ProjT Launcher is made by and for the community. Every feature we ship is reviewed in the open, and you can remix or redistribute it with zero lock-in.</p>
    </div>
    <div class="stats-grid">
      <div class="stat-card">
        <h3>Open</h3>
        <p>GPL-3 licensed launcher &amp; AGPL-3 website</p>
      </div>
      <div class="stat-card">
        <h3>3</h3>
        <p>Platforms: Windows, macOS &amp; Linux</p>
      </div>
      <div class="stat-card">
        <h3>Unlimited</h3>
        <p>Instances with isolated resource packs, mods, and saves</p>
      </div>
      <div class="stat-card">
        <h3>0</h3>
        <p>Advertising, trackers, or paywalls</p>
      </div>
    </div>
  </section>

  <section class="section community-section" id="get-involved">
    <div class="section-heading">
      <p class="eyebrow">Community first</p>
      <h2>Get involved &amp; stay up to date</h2>
    </div>
    <div class="community-grid">
      <article class="community-card">
        <h3>Matrix Space</h3>
        <p>Prefer open chat instead of another Discord server? Join the Matrix Space to talk features, get help, or share your modpack.</p>
        <a class="button type-link size-small" href="https://matrix.to/#/#projtlauncher:matrix.org" target="_blank">Join on Matrix</a>
      </article>
      <article class="community-card">
        <h3>Source code</h3>
        <p>ProjT Launcher is GPL-3 and accepts contributions from anyone who cares about user freedom. Fork it, remix it, or ship your own build.</p>
        <a class="button ghost-button size-small" href="https://github.com/Project-Tick/ProjT-Launcher" target="_blank">Explore on GitHub</a>
        <p class="eyebrow">Website: <a href="https://github.com/Project-Tick/projtlauncher.yongdohyun.org.tr">AGPL-3</a></p>
      </article>
      <article class="community-card">
        <h3>Docs &amp; Wiki</h3>
        <p>New to ProjT Launcher? The wiki covers everything from first launch to changing themes or building your own mod catalog.</p>
        <a class="button type-info size-small" href="{{ '/wiki/' | url }}">Browse the wiki</a>
      </article>
    </div>
  </section>

  <section class="section faq-section" id="faq">
    <div class="section-heading">
      <p class="eyebrow">FAQ</p>
      <h2>Questions from fellow modded players</h2>
    </div>
    <div class="faq-list">
      <details class="faq-item" open>
        <summary>Does ProjT Launcher support official Microsoft accounts?</summary>
        <p>Yes. You can authenticate multiple Microsoft accounts side by side while keeping Mojang accounts available for legacy instances.</p>
      </details>
      <details class="faq-item">
        <summary>How does telemetry work?</summary>
        <p>It doesn't. We never ship tracking or ads, and we keep the source open so you can verify that promise.</p>
      </details>
      <details class="faq-item">
        <summary>Can I move my Prism Launcher instances over?</summary>
        <p>Absolutely. The wiki has a full migration guide, and ProjT can import worlds, mod lists, and even screenshots from existing folders.</p>
      </details>
      <details class="faq-item">
        <summary>Is there a CLI?</summary>
        <p>Yes. The launcher exposes a command-line interface for automating downloads, managing Catpacks, or scripting testing flows.</p>
      </details>
    </div>
  </section>

  <section class="section cta-panel">
    <p class="eyebrow">Ready when you are</p>
    <h2>Download ProjT Launcher and reclaim your modded workflow</h2>
    <p>One launcher, every platform, and a community that cares about software freedom. No telemetry, no ads—just Minecraft the way you want it.</p>
    <a class="button size-large" href="{{ '/download?from=button' | url }}">Download now</a>
  </section>
</div>
