# =============================================================================
# ProjT Launcher - GitHub Step Summary Generator
# =============================================================================
# Generates markdown summary for GitHub Actions workflow steps.
# =============================================================================

# Truncate long lists for readability
def truncate(xs; n):
  if xs | length > n then xs[:n] + ["..."]
  else xs
  end;

# Format a list of files as markdown
def itemize_files(xs):
  truncate(xs; 50) |
    map("- `\(.)`") |
    join("\n");

# Get title with count
def get_title(s; xs):
  s + " (" + (xs | length | tostring) + ")";

# Create collapsible section
def section(title; xs):
  if xs | length == 0 then ""
  else
    "<details>\n<summary>" + get_title(title; xs) + "</summary>\n\n" + itemize_files(xs) + "\n</details>"
  end;

# Generate platform status row
def platform_row(name; status):
  "| " + name + " | " + (if status then "✅ Ready" else "⏳ Pending" end) + " |";

# Main summary generator
def generate_summary:
  "## ProjT Launcher - Build Change Summary\n\n" +
  
  "### Changed Files\n\n" +
  section("Core Changes"; .categories.core // []) + "\n\n" +
  section("UI Changes"; .categories.ui // []) + "\n\n" +
  section("Minecraft Changes"; .categories.minecraft // []) + "\n\n" +
  section("Build System Changes"; .categories.build // []) + "\n\n" +
  section("Dependency Changes"; .categories.dependencies // []) + "\n\n" +
  section("Documentation Changes"; .categories.docs // []) + "\n\n" +
  section("CI Changes"; .categories.ci // []) + "\n\n" +
  section("Translation Changes"; .categories.translations // []) + "\n\n" +
  
  "### Platform Status\n\n" +
  "| Platform | Status |\n" +
  "|----------|--------|\n" +
  platform_row("Linux"; .platforms.linux // true) + "\n" +
  platform_row("macOS"; .platforms.macos // true) + "\n" +
  platform_row("Windows"; .platforms.windows // true) + "\n\n" +
  
  "### Build Impact\n\n" +
  (if .rebuildRequired then
    "⚠️ **Rebuild Required**: Changes affect build output\n"
  else
    "✅ **No Rebuild Required**: Changes don't affect build\n"
  end) +
  
  "\n### Labels\n\n" +
  (if .labels | length > 0 then
    (.labels | to_entries | map("- `\(.key)`") | join("\n"))
  else
    "No labels assigned"
  end);

# Entry point
generate_summary
