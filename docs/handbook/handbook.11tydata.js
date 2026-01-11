module.exports = {
  layout: "handbook.njk",
  eleventyComputed: {
    permalink: function(data) {
      // docs/handbook/zlib.md -> /handbook/zlib/index.html
      let filename = data.page.fileSlug;
      if (filename === "README") {
        return false; // Skip README, we have our own index
      }
      return `/handbook/${filename}/index.html`;
    }
  }
};
