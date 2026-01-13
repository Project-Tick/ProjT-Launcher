cask "projtlauncher" do
  version "0.0.3-4"
  depends_on macos: :big_sur

  sha256 "68ff5bdaf1e92675a4a660e5b6bf42a989aae30dd6680856e7f691ea44809aec"

  url "https://github.com/Project-Tick/ProjT-Launcher/releases/download/#{version}/ProjTLauncher-macOS-#{version}.zip",
    verified: "github.com/Project-Tick/ProjT-Launcher/"

  name "ProjT Launcher"
  desc "Minecraft launcher"
  homepage "https://projecttick.org/projtlauncher/"

  livecheck do
    url "https://projecttick.org/projtlauncher/feed/appcast.xml"
    strategy :sparkle
  end

  auto_updates true

  app "ProjT Launcher.app"
  binary "#{appdir}/ProjT Launcher.app/Contents/MacOS/projtlauncher"

  zap trash: [
    "~/Library/Application Support/ProjT Launcher/metacache",
    "~/Library/Application Support/ProjT Launcher/projtlauncher-*.log",
    "~/Library/Application Support/ProjT Launcher/projtlauncher.cfg",
    "~/Library/Preferences/org.projecttick.ProjTLauncher.plist",
    "~/Library/Saved Application State/org.projecttick.ProjTLauncher.savedState",
  ]
end