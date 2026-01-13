module.exports = {
  layout: "handbook.njk",
  eleventyComputed: {
    permalink: function (data) {
      const input = data.page.inputPath || "";
      const stemRaw = (data.page.filePathStem || "").replace(/^\/+/, "");

      // Skip README
      if (stemRaw.endsWith("README")) {
        return false;
      }

      // Avoid clash between help-pages overview files
      if (input.endsWith("/docs/handbook/help-pages.md")) {
        return "/handbook/help-pages/overview/index.html";
      }

      // Normalize path: strip leading docs/handbook/ or handbook/
      let trimmed = stemRaw.replace(/^docs\/handbook\//, "").replace(/^handbook\//, "");

      // Collapse directory index to folder path
      trimmed = trimmed.replace(/\/index$/, "");

      return `/handbook/${trimmed}/index.html`;
    },
  },
};
