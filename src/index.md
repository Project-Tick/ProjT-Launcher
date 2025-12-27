---
layout: layouts/main.njk
title: Home
hero:
  title: 'ProjT Launcher'
  description: 'An open source Minecraft launcher with the ability to manage multiple instances, accounts and mods. Focused on user freedom and free redistributability.'
  image:
    light: '/img/screenshots/projtlauncher_dark_main_window.png'
    dark: '/img/screenshots/projtlauncher_dark_main_window.png'
  button:
    url: /download?from=button
    content: 'Download'
---

<section class="features-section">
  <div class="features-header">
    <h2>Everything you need.<br><span class="gradient-text">Nothing you don't.</span></h2>
  </div>
  
  <div class="features-grid">
    <div class="feature-card">
      <div class="feature-icon"><i class="fa fa-cubes"></i></div>
      <h3>Modpack Comfort</h3>
      <p>Easily install and update modpacks from Modrinth, CurseForge, ATLauncher, Technic, and FTB. Import from zip files or creating one from scratch.</p>
    </div>

    <div class="feature-card">
      <div class="feature-icon"><i class="fa fa-refresh"></i></div>
      <h3>Smart Updates</h3>
      <p>ProjT Launcher will check for updates every time you launch it. No need to manually download anything.</p>
    </div>
    
    <div class="feature-card">
      <div class="feature-icon"><i class="fa fa-hdd-o"></i></div>
      <h3>Isolated Instances</h3>
      <p>Each instance is stored in its own folder. This means you can have different mods, resource packs, and saves for each instance.</p>
    </div>

    <div class="feature-card">
      <div class="feature-icon"><i class="fa fa-paint-brush"></i></div>
      <h3>Themeable</h3>
      <p>Customize the launcher look and feel with Qt themes and CSS. Make it truly yours.</p>
    </div>

    <div class="feature-card">
      <div class="feature-icon"><i class="fa fa-bolt"></i></div>
      <h3>Lightweight</h3>
      <p>Built with C++ and Qt for maximum performance and minimum resource usage. Fast startup, low memory footprint.</p>
    </div>
    
    <div class="feature-card">
      <div class="feature-icon"><i class="fa fa-github"></i></div>
      <h3>Open Source</h3>
      <p>Licensed under GPL-3.0. You can inspect the code, modify it, and redistribute it freely.</p>
    </div>

    <div class="feature-card">
      <div class="feature-icon"><i class="fa fa-th-large"></i></div>
      <h3>Unlimited</h3>
      <p>Instances with isolated resource packs, mods, and saves.</p>
    </div>
  </div>
</section>

<section class="showcase-section">
  <div class="showcase-header">
    <span class="eyebrow">Ecosystem</span>
    <h2>Integrated with the best</h2>
  </div>
  
  <div class="features-grid">
    <div class="feature-card">
      <div class="feature-icon"><i class="fa fa-search"></i></div>
      <h3>Modrinth & CurseForge</h3>
      <p>Direct download and update support for mods and modpacks from the largest community repositories.</p>
    </div>

    <div class="feature-card">
      <div class="feature-icon"><i class="fa fa-paw"></i></div>
      <h3>Catpacks</h3>
      <p>First-class support for Catpacks, a new modpack format focused on distribution and ease of use.</p>
    </div>
    
    <div class="feature-card">
      <div class="feature-icon"><i class="fa fa-comments"></i></div>
      <h3>Community Support</h3>
      <p>Join our Matrix community for help, discussions, and development updates.</p>
    </div>
  </div>
</section>

<section class="cta-section">
  <div class="cta-content">
    <h2>Why players switch?</h2>
    <div class="features-grid" style="margin-top: 40px; margin-bottom: 40px;">
      <div class="feature-card">
        <h3>01. Speed</h3>
        <p>Native C++ application means instant startup and minimal background usage.</p>
      </div>
      <div class="feature-card">
        <h3>02. Privacy</h3>
        <p>No telemetry, no tracking, no account requirements for basic usage.</p>
      </div>
      <div class="feature-card">
        <h3>03. Control</h3>
        <p>Full control over Java versions, memory allocation, and JVM arguments.</p>
      </div>
    </div>

    <h2>Ready to play?</h2>
    <p>Download ProjT Launcher and take control of your Minecraft experience.</p>
    <div class="cta-actions">
      <a href="{{ '/download?from=button' | url }}" class="btn btn-white btn-lg"><i class="fa fa-download"></i> Download Now</a>
      <a href="{{ '/wiki/' | url }}" class="btn btn-outline btn-lg">Read the Docs</a>
    </div>
  </div>
</section>
