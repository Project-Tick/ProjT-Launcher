---
permalink: /handbook/index.html
eleventyExcludeFromCollections: true
---
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Developer Handbook</title>
  <style>body{font-family:-apple-system,BlinkMacSystemFont,"Segoe UI",Helvetica,Arial,sans-serif;font-size:15px;line-height:1.6;color:#000;background:#fff;max-width:72ch;margin:0 auto;padding:20px}a{color:#00e}a:visited{color:#609}a:hover{color:#c00}h1{font-size:1.4em;border-bottom:1px solid #000;padding-bottom:5px}h2{font-size:1.2em;margin-top:1.5em}h3{font-size:1em;margin-top:1em}p{margin:.5em 0}hr{border:0;border-top:1px solid #ccc;margin:1.5em 0}ul{margin:.5em 0 .5em 2em}li{margin:.3em 0}.nav{margin-bottom:1.5em}.nav a{margin-right:1em}</style>
</head>
<body>
  <div class="nav"><a href="/">Project Tick</a> <a href="/projtlauncher/">ProjT Launcher</a></div>

  <h1>Developer Handbook</h1>
  <p>Documentation for ProjT Launcher contributors.</p>

  <hr>

  <h2>Core Components</h2>
  <ul>
    <li><a href="/handbook/program_info/">Program Info</a> - Application branding, URLs</li>
    <li><a href="/handbook/launcherjava/">LauncherJava</a> - Java launcher component</li>
    <li><a href="/handbook/javacheck/">JavaCheck</a> - Java runtime detection</li>
  </ul>

  <h2>Bundled Libraries (Detached Forks)</h2>

  <h3>Compression</h3>
  <ul>
    <li><a href="/handbook/zlib/">zlib</a> - PNG/DEFLATE compression</li>
    <li><a href="/handbook/bzip2/">bzip2</a> - Block-sorting compression</li>
    <li><a href="/handbook/quazip/">QuaZip</a> - Qt ZIP wrapper</li>
  </ul>

  <h3>Data Formats</h3>
  <ul>
    <li><a href="/handbook/tomlplusplus/">toml++</a> - TOML parser for C++17</li>
    <li><a href="/handbook/libnbtplusplus/">libnbt++</a> - Minecraft NBT format</li>
    <li><a href="/handbook/cmark/">cmark</a> - CommonMark Markdown parser</li>
    <li><a href="/handbook/libqrencode/">libqrencode</a> - QR code generation</li>
  </ul>

  <h2>CI/CD & Automation</h2>
  <ul>
    <li><a href="/handbook/workflows/">Workflows</a> - GitHub Actions architecture</li>
    <li><a href="/handbook/ci_support/">CI Support</a> - Configuration files</li>
    <li><a href="/handbook/bot/">Bot</a> - PR automation</li>
  </ul>

  <h2>Platform Support</h2>
  <ul>
    <li><a href="/handbook/nix/">Nix Packaging</a> - NixOS/Nix flake guide</li>
  </ul>

  <hr>

  <p>Source: <a href="https://github.com/Project-Tick/ProjT-Launcher/tree/develop/docs/handbook">docs/handbook/</a></p>
  <p><a href="/">Back to Project Tick</a></p>
</body>
</html>
