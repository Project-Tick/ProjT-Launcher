## Overview: What Is Project Tick and Why It Exists?
Project Tick is a free software ecosystem designed for user freedom, legal clarity, and long term sustainability. It exists as a disciplined and pragmatic answer to the most common failure modes in open source. Open source projects often collapse due to unclear technical direction, rushed architectural decisions, and neglected CI/packaging pipelines. Project Tick was created to avoid those mistakes: decisions are documented, automation and reproducible builds are mandatory, and long term correctness wins over short term speed.

This document summarizes Project Tick's purpose, core principles, products, infrastructure, and community model. It is both a description of how we work and a clear view of where we are headed.

## Core Principles: Discipline and a Long Term Outlook
Project Tick rejects the "move fast, break fast" mindset in favor of durable and scalable solutions. Core principles:

- Long Term Maintenance First: Code quality is never sacrificed for speed. Long term maintenance is more valuable than short term velocity.
- Enforced Architectural Boundaries: Architecture is explicitly defined and enforced. Disciplined modules beat fuzzy boundaries.
- CI/CD and Automation: Continuous integration and automation are first class requirements. Everything possible should be automated.
- Reproducible Builds: Build steps, inputs, and dependencies are deterministic so outputs match across environments.
- Respect for Licenses and Contributions: Upstream projects, licenses, and contributor intent are treated with care. Every change goes through technical and professional review; changes without clear rationale are rejected.

These principles challenge the idea that "projects decay over time" and aim to keep the codebase resilient.

## Why Project Tick? A Remedy to Open Source Pain Points
Common failures in open source include unclear direction, rushed architecture, neglected CI and packaging, undocumented changes, and hidden breakages. Project Tick stands against these with one rule: long term correctness is superior to short term comfort. Decisions are documented, automation is mandatory, and every change is questioned. This minimizes inconsistency and technical debt.

## Product Ecosystem
Project Tick is not a single project; it is a disciplined software ecosystem. Major products and components include:

### ProjT Launcher - Flagship
ProjT Launcher is the flagship product, a cross platform Minecraft launcher built for long term maintenance and architectural clarity. It is a fork of Prism Launcher focused on discipline and sustainability.

- Long Term Maintenance: Architectural boundaries and review rules prevent uncontrolled technical debt.
- Controlled Third Party Integration: External dependencies, patch policies, and update procedures are maintained as documented, isolated forks.
- Deterministic CI and Builds: Dependency versions and build inputs are pinned for identical results across environments.
- Architectural Clarity: MVVM boundaries and a modular layout make review, refactoring, and long term contribution easier.

ProjT Launcher is built with C++ and Qt, and provides reproducible builds via Nix and CMake. The repository is split into modules such as `launcher/` (app), `website/` (Eleventy site), `bot/` (automation), `meta/` (metadata generator), and `docs/` (documentation). The project vendors many forked libraries, including zlib, bzip2, quazip, cmark, tomlplusplus, and libqrencode. Other vendored components include GameMode integration, LocalPeer, murmur2 hashing, and terminal color utilities.

### LauncherJava
LauncherJava is the Java component that actually starts Minecraft. It acts as the interface between the C++ launcher and the Minecraft JVM process. The launcher receives commands via a text based protocol, such as `mainClass`, `param`, `windowTitle`, and `sessionId`. There are two launcher types: standard (all modern versions) and legacy (pre 1.6). LauncherJava is built via CMake and can also be built with `./gradlew build`.

### JavaCheck
JavaCheck is a minimal Java program that prints system properties. ProjT Launcher uses it to detect and validate Java installs, check runtime availability, version, architecture (32/64 bit), and auto detect installed JREs/JDKs.

### bzip2 and zlib
ProjT Launcher maintains controlled forks of compression libraries.

- bzip2/libbz2: Lossless block sorted compression. The fork stays compatible with upstream while moving forward in a controlled tree. Used for mod archive extraction, legacy Minecraft asset support, and large file download optimizations.
- zlib: Implements DEFLATE and provides ZIP/GZip functionality. Used for zip archive handling, PNG texture compression, network compression for mod downloads, and Minecraft world data (NBT). The fork is kept for long term maintenance and CI validation.

These components are foundational for deterministic and high performance behavior.

## Forge (Git) and Independent Infrastructure
Project Tick hosts its code on its own Forge platform. The "Sovereign Code" approach avoids dependence on third party platforms. Forge is optimized for high availability and speed, globally accessible, and community oriented. Contributors can join projects, fork, and collaborate in a transparent environment.

## Mailing Lists and Community
Project Tick uses mailing lists for clarity and transparency. Each list has a clear purpose:

- Announcements: Official announcements, release notices, security advisories, and major updates. Low traffic for readers, posting is restricted.
- Development (devel): Technical discussions, patch submissions, RFCs, architectural decisions, and infrastructure topics.
- Community: General discussion, questions, ideas, feedback, and coordination. The right starting point for newcomers.

The community hub also contains categories for bug reports and PR review requests, but currently has no discussion threads.

## Roadmap and Status
As of 2026, the roadmap is intentionally small and focused. Work is active on completing the GNU Make system. One completed item is a reported issue in the ProjT Launcher update system, announced to be fixed in version 0.0.4. No other items are planned, reflecting the "few but solid" approach.

System status shows 100% uptime for global infrastructure; the mailing list system averages 1 ms latency with 100.0% uptime. No incidents were reported in the last 24 hours. This reflects a strong focus on stability.

## Licensing and Legal Framework
Project Tick is ambitious not only in software, but in licensing as well. In addition to standard open source licenses, it offers specialized licenses for the AI and data era.

### Project Tick General Public License (PT-GPL)
PT-GPL v1 is a copyleft license for software programs. It guarantees user freedoms to use, run, modify, and share the software, requires the license to accompany copies, and preserves copyright notices. It emphasizes access to reproducible forms and passing modification rights downstream. The preamble balances flexible use rights with the protection of authors and contributors.

### Project Tick AI Use License (PT-AI)
PT-AI v2 defines how works may be used by AI systems. It prohibits "reconstruction" (reproducing a meaningful portion of a work). Allowed AI uses include indexing, classification, search, temporary analysis, and inference only. Training is permitted only for non commercial research, and deliberate storage in model parameters is forbidden. Commercial AI training is prohibited or requires a separate agreement.

### Privacy Policy and Terms of Service
PT-PP (Privacy Policy) v2 guarantees minimal data collection and no sale of personal data. Collected data is limited to account information, form submissions, and technical data (IP, browser, OS). Data is used only for service operation, account management, security, and legal obligations. Cookies are used only for session management, authentication, and security; no ads or cross site tracking.

PT-TOS (Terms of Service) v2 states that everyone accessing the website must follow these terms; they are separate from software licenses. Project Tick may restrict access for misuse. User submitted content may be used royalty free for operations and development; intellectual property rights belong to Project Tick, and no warranty or liability is provided.

These documents define clear boundaries for both developers and users.

## Design and Brand Identity
Project Tick visual identity is sharp, aggressive, and industrial. Color palette and typography:

| Element | Description | Notes |
| --- | --- | --- |
| PT Red | #FF003C | Used for technical highlights and warnings |
| Deep Black | #050505 | Signature dark background for high contrast |
| Light Gray | #F0F0F0 | Light surfaces and reading areas |
| Slate Gray | #6A6E73 | Mid tone separation |
| Display Font | Space Grotesk | Geometric sans serif for titles and UI |
| Text Font | Red Hat Text | High readability for technical docs |

Design principles include sharp, aggressive lines (0px border radius), high contrast, and a dark mode first approach. This aesthetic reflects a professional and serious stance.

## New Website and Future Vision
On January 27, 2026, Project Tick launched a rebuilt website. This was not only a visual refresh but a dynamic foundation for a growing ecosystem. Highlights include:

- Unified product structure: products, licenses, docs, and announcements are organized under one coherent model.
- Licensing and legal clarity: canonical license pages, version control, and dedicated areas for Terms and Privacy.
- Contributor management: a GitHub based Contributor License Agreement (CLA) system.
- Dynamic content system: products, handbooks, and news are managed dynamically for long term scalability and fast updates.
- Cleaner design: reduced noise, easier navigation, and clear separation between open source projects and platform services.

The site provides a stable base for future products and features, with new additions planned incrementally. This reinforces the idea that Project Tick is not finished; it is a beginning.

## Conclusion
Project Tick is an ecosystem built to address the lack of sustainability and discipline in open source. With a small but critical roadmap and a stable, transparent infrastructure, it offers a clear proposal: choose long term stability over short term speed, and disciplined evolution over random novelty.

This overview outlines Project Tick's principles, products, and future. If you are considering joining, know that contributions are evaluated against high quality standards, clear justification, and long term maintenance requirements. Project Tick is not just a collection of software, but a clear vision of how open source should be.
