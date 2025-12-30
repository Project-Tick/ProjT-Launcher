const { DateTime } = require("luxon");
const fs = require("fs");
const path = require("path");
const markdownIt = require("markdown-it");
const markdownItAnchor = require("markdown-it-anchor");
const markdownItTableOfContents = require("markdown-it-table-of-contents");
const cleanCSS = require("clean-css");
const Image = require("@11ty/eleventy-img");

const DIRS = {
  input: "website",
  output: "_site",
  imagesBuilt: "img/built",
};

const IMAGE_OUTPUT_DIR = path.join(DIRS.output, DIRS.imagesBuilt);
const IMAGE_URL_PATH = `/${DIRS.imagesBuilt}/`;

async function image(alt, filepath, darkpath, classes, lossless = true, sizes = "100vw") {
  if (alt === undefined) {
    // You bet we throw an error on missing alt (alt="" works okay)
    throw new Error(`Missing \`alt\` on responsiveimage from: ${filepath}`);
  }

  let options = {
    widths: [null],
    formats: lossless ? ["webp", "png"] : ["webp", "jpeg"],
    urlPath: IMAGE_URL_PATH,
    outputDir: IMAGE_OUTPUT_DIR,
    sharpWebpOptions: {
      quality: 70,
      lossless: lossless,
      effort: 6,
    },
    sharpPngOptions: {
      compressionLevel: 9,
    },
    sharpJpegOptions: {
      quality: 70,
      progressive: true,
      mozjpeg: true,
    },
  };

  var metadata_dark = {};
  if (darkpath) {
    metadata_dark = await Image(darkpath, options);
  }

  let metadata = await Image(filepath, options);

  let lowwebsite = lossless ? metadata.png[0] : metadata.jpeg[0];
  let highwebsite = lossless ? metadata.png[metadata.png.length - 1] : metadata.jpeg[metadata.jpeg.length - 1];

  return `<picture>
    ${Object.values(metadata_dark)
      .map((imageFormat) => {
        return `  <source type="${imageFormat[0].sourceType
          }" websiteset="${imageFormat
            .map((entry) => entry.websiteset)
            .join(", ")}" sizes="${sizes}" media="(prefers-color-scheme: dark)">`;
      })
      .join("\n")}
    ${Object.values(metadata)
      .map((imageFormat) => {
        return `  <source type="${imageFormat[0].sourceType
          }" websiteset="${imageFormat
            .map((entry) => entry.websiteset)
            .join(", ")}" sizes="${sizes}">`;
      })
      .join("\n")}
      <img
        class="${classes}"
        website="${lowwebsite.url}"
        width="${highwebsite.width}"
        height="${highwebsite.height}"
        alt="${alt}"
        loading="lazy"
        decoding="async">
    </picture>`;
}

module.exports = async function (eleventyConfig) {
  const { EleventyRenderPlugin } = await import("@11ty/eleventy");
  const pluginRss = await import("@11ty/eleventy-plugin-rss");
  const pluginSyntaxHighlight = await import("@11ty/eleventy-plugin-syntaxhighlight");
  const pluginNavigation = await import("@11ty/eleventy-navigation");

  eleventyConfig.addPassthroughCopy("website/img");
  eleventyConfig.addPassthroughCopy("website/admin");
  eleventyConfig.addPassthroughCopy("website/welcome-channel.yaml");
  eleventyConfig.addPassthroughCopy("website/favicon.ico");
  eleventyConfig.addPassthroughCopy("website/.well-known");
  eleventyConfig.addPassthroughCopy("website/_redirects"); // Netlify redirects
  eleventyConfig.addPassthroughCopy({
    "node_modules/@fontsource/inter": "assets/fonts/inter",
  });
  eleventyConfig.addPassthroughCopy({
    "website/_includes/components/forkawesome": "assets/forkawesome",
  });
  


  // Add plugins
  eleventyConfig.addPlugin(pluginRss.default);
  eleventyConfig.addPlugin(pluginSyntaxHighlight.default);
  eleventyConfig.addPlugin(pluginNavigation.default);
  eleventyConfig.addPlugin(EleventyRenderPlugin);

  // Add shortcodes
  eleventyConfig.addNunjucksAsyncShortcode("image", image);

  eleventyConfig.addPairedShortcode("markdown", (content) => {
    return markdownLibrary.render(content);
  });

  // Alias `layout: post` to `layout: layouts/post.njk`
  eleventyConfig.addLayoutAlias("post", "layouts/post.njk");

  eleventyConfig.addFilter("cssmin", function (code) {
    return new cleanCSS({}).minify(code).styles;
  });

  eleventyConfig.addFilter("readableDate", (dateObj) => {
    return DateTime.fromJSDate(dateObj, { zone: "utc" }).toFormat(
      "dd LLL yyyy"
    );
  });

  // https://html.spec.whatwg.org/multipage/common-microsyntaxes.html#valid-date-string
  eleventyConfig.addFilter("htmlDateString", (dateObj) => {
    return DateTime.fromJSDate(dateObj, { zone: "utc" }).toFormat("yyyy-LL-dd");
  });

  // Get the first `n` elements of a collection.
  eleventyConfig.addFilter("head", (array, n) => {
    if (!Array.isArray(array) || array.length === 0) {
      return [];
    }
    if (n < 0) {
      return array.slice(n);
    }

    return array.slice(0, n);
  });

  // Return the smallest number argument
  eleventyConfig.addFilter("min", (...numbers) => {
    return Math.min.apply(null, numbers);
  });

  function filterTagList(tags) {
    return (tags || []).filter(
      (tag) => ["all", "nav", "post", "posts", "wiki"].indexOf(tag) === -1
    );
  }

  eleventyConfig.addFilter("filterTagList", filterTagList);

  // Create an array of all tags
  eleventyConfig.addCollection("tagList", function (collection) {
    let tagSet = new Set();
    collection.getAll().forEach((item) => {
      (item.data.tags || []).forEach((tag) => tagSet.add(tag));
    });

    return filterTagList([...tagSet]);
  });

  // Customize Markdown library and settings:
  let markdownLibrary = markdownIt({
    html: true,
    breaks: true,
    linkify: true,
  }).use(markdownItAnchor, {
    permalink: markdownItAnchor.permalink.ariaHidden({
      placement: "after",
      class: "direct-link",
      symbol: "#",
      level: [1, 2, 3, 4],
    }),
    slugify: eleventyConfig.getFilter("slug"),
  }).use(markdownItTableOfContents, {"includeLevel": [1,2,3]});
  eleventyConfig.setLibrary("md", markdownLibrary);

  return {
    // Control which files Eleventy will process
    // e.g.: *.md, *.njk, *.html, *.liquid
    templateFormats: ["md", "njk", "html"],

    // Pre-process *.md files with: (default: `liquid`)
    markdownTemplateEngine: "njk",

    // Pre-process *.html files with: (default: `liquid`)
    htmlTemplateEngine: "njk",

    dir: {
      input: DIRS.input,
      includes: "_includes",
      data: "_data",
      output: DIRS.output,
    },
  };
};
