module.exports = {
  layout: "handbook.njk",
  permalink: function (data) {
    const input = (data.page.inputPath || "").replace(/\\/g, "/");
    const stemRaw = (data.page.filePathStem || "").replace(/\\/g, "/").replace(/^\/+/, "");

    // Skip README
    if (stemRaw.endsWith("README")) {
      return false;
    }

    // Avoid clash between help-pages overview files
    if (input.endsWith("/website-root/handbook/help-pages.md") || input.endsWith("/handbook/help-pages.md")) {
      return "/handbook/help-pages/overview/index.html";
    }

    // Normalize path: strip leading website-root/handbook/ or handbook/
    let trimmed = stemRaw
      .replace(/^website-root\/handbook\//, "")
      .replace(/^handbook\//, "");

    // Collapse directory index to folder path
    trimmed = trimmed.replace(/\/index$/, "");

    // Root index should be /handbook/index.html
    if (trimmed === "index") {
      return "/handbook/index.html";
    }

    return `/handbook/${trimmed}/index.html`;
  },
};
