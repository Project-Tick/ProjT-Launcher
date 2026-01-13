module.exports = {
  permalink: function(data) {
    const rawInput = data.page.inputPath || "";
    const inputPath = rawInput.replace(/\\/g, "/");
    if (inputPath.startsWith("./website-root/handbook/") || inputPath.startsWith("website-root/handbook/")) {
      return undefined;
    }
    if (inputPath.startsWith("./website-root/")) {
      return inputPath.replace("./website-root", "");
    }
    if (inputPath.startsWith("website-root/")) {
      return inputPath.replace("website-root", "");
    }
    return false;
  }
};
