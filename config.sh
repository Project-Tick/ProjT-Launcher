export META_UPSTREAM_URL="${META_UPSTREAM_URL:-git@github.com:Project-Tick/meta-upstream.git}"
export META_LAUNCHER_URL="${META_LAUNCHER_URL:-git@github.com:Project-Tick/meta-launcher.git}"


export META_UPSTREAM_DIR="${META_UPSTREAM_DIR:-upstream}"
export META_LAUNCHER_DIR="${META_LAUNCHER_DIR:-launcher}"
export DEPLOY_TO_FOLDER="${DEPLOY_TO_FOLDER:-false}"
export DEPLOY_FOLDER="${DEPLOY_FOLDER:-/app/public/v1}"
export DEPLOY_FOLDER_USER="${DEPLOY_FOLDER_USER:-http}"
export DEPLOY_FOLDER_GROUP="${DEPLOY_FOLDER_GROUP:-http}"

export UPSTREAM_SSH="${UPSTREAM_SSH:-/home/samet/meta/config/key_upstream}"
export LAUNCHER_SSH="${LAUNCHER_SSH:-/home/samet/meta/config/key_launcher}"
export DEPLOY_TO_GIT="${DEPLOY_TO_GIT:-true}"
export GIT_AUTHOR_NAME="${GIT_AUTHOR_NAME:-Project Tick Bot}"
export GIT_AUTHOR_EMAIL="${GIT_AUTHOR_EMAIL:-github@bot.yongdohyun.org.tr}"
export GIT_COMMITTER_NAME="${GIT_COMMITTER_NAME:-$GIT_AUTHOR_NAME}"
export GIT_COMMITTER_EMAIL="${GIT_COMMITTER_EMAIL:-$GIT_AUTHOR_EMAIL}"
