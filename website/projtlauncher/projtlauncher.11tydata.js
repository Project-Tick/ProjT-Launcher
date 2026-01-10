module.exports = {
  permalink: function(data) {
    let inputPath = data.page.inputPath;
    if (inputPath.startsWith("./website/projtlauncher/")) {
      let path = inputPath.replace("./website/projtlauncher", "/projtlauncher");
      return path.replace(/\.md$/, ".html").replace(/\.njk$/, ".html");
    }
    return false;
  }
};
