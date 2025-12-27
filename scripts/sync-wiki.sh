#!/bin/bash

# Configuration
WIKI_REPO_URL="git@github.com:grxtor/ProjT-Launcher.wiki.git"
WIKI_LOCAL_DIR=".wiki-local"
SOURCE_DIR="src/wiki"

# Ensure source directory exists
if [ ! -d "$SOURCE_DIR" ]; then
    echo "Error: Source directory '$SOURCE_DIR' does not exist."
    exit 1
fi

# Clone wiki repo if not already cloned
if [ ! -d "$WIKI_LOCAL_DIR" ]; then
    echo "Cloning wiki repository..."
    git clone "$WIKI_REPO_URL" "$WIKI_LOCAL_DIR"
    if [ $? -ne 0 ]; then
        echo "Error: Failed to clone wiki repository. Make sure you have the correct permissions and the Wiki feature is enabled in GitHub settings."
        exit 1
    fi
else
    echo "Pulling latest changes..."
    cd "$WIKI_LOCAL_DIR" && git pull && cd ..
fi

# Sync files
echo "Syncing files..."
# Use rsync if available, otherwise fallback to rm+cp
if command -v rsync &> /dev/null; then
    rsync -av --delete --exclude '.git' "$SOURCE_DIR/" "$WIKI_LOCAL_DIR/"
else
    # Simple cleaner copy (less robust than rsync but sufficient)
    rm -rf "$WIKI_LOCAL_DIR"/*
    cp -R "$SOURCE_DIR/"* "$WIKI_LOCAL_DIR/"
fi

# Commit and push
cd "$WIKI_LOCAL_DIR"

# Configure git identity for this repo
git config user.name "grxtor"
git config user.email "97219311+YongDo-Hyun@users.noreply.github.com"

if [ -n "$(git status --porcelain)" ]; then
    echo "Changes detected. Committing and pushing..."
    git add .
    git commit -m "Update wiki from src/wiki"
    git push origin master
    echo "Wiki updated successfully!"
else
    echo "No changes to sync."
fi
