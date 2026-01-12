module.exports = {
  permalink: function(data) {
    let inputPath = data.page.inputPath;
    if (inputPath.startsWith("./website-root/")) {
      return inputPath.replace("./website-root", "");
    }
    return false;
  }
};
