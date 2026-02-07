#!/bin/bash
# ============================================================================
# syncconfig.sh - Generate auto.conf and autoconf.h from .config
# ============================================================================
#
# This is a lightweight alternative to kconfig's syncconfig for platforms
# where kconfig tools cannot be built (Windows, macOS CI, etc.).
#
# It reads a .config file and generates:
#   - include/config/auto.conf   (Makefile-includable CONFIG_* variables)
#   - include/generated/autoconf.h (C header with #define CONFIG_*)
#
# Usage:
#   syncconfig.sh <build-dir>
#
# ============================================================================

set -e

# Force C locale for consistent regex matching across all locales.
# Turkish locale (tr_TR) breaks [A-Za-z] character ranges in grep.
export LC_ALL=C

BUILD_DIR="${1:-.}"
CONFIG_FILE="${BUILD_DIR}/.config"
AUTO_CONF_DIR="${BUILD_DIR}/include/config"
AUTO_HEADER_DIR="${BUILD_DIR}/include/generated"
AUTO_CONF="${AUTO_CONF_DIR}/auto.conf"
AUTO_HEADER="${AUTO_HEADER_DIR}/autoconf.h"

if [ ! -f "$CONFIG_FILE" ]; then
    echo "error: $CONFIG_FILE not found" >&2
    exit 1
fi

# Create directories
mkdir -p "$AUTO_CONF_DIR"
mkdir -p "$AUTO_HEADER_DIR"

# ============================================================================
# Generate auto.conf
# ============================================================================

{
    echo "# Automatically generated - do not edit"
    echo "# $(date)"
    echo ""
    # Extract all CONFIG_*=value lines (skip comments and blank lines)
    grep -E '^CONFIG_[A-Za-z0-9_]+=' "$CONFIG_FILE" || true
    # Also extract non-CONFIG lines that are make variable assignments
    grep -E '^[A-Z_]+[[:space:]]*:?=' "$CONFIG_FILE" | grep -v '^CONFIG_' || true
} > "$AUTO_CONF"

echo "  GEN     $AUTO_CONF"

# ============================================================================
# Generate autoconf.h
# ============================================================================

{
    echo "/* Automatically generated - do not edit */"
    echo "#ifndef __AUTOCONF_H__"
    echo "#define __AUTOCONF_H__"
    echo ""

    while IFS= read -r line; do
        # Skip comments and blank lines
        case "$line" in
            '#'*|'') continue ;;
        esac

        # Handle CONFIG_*=y  -> #define CONFIG_* 1
        if echo "$line" | grep -qE '^CONFIG_[A-Za-z0-9_]+=y$'; then
            key=$(echo "$line" | cut -d= -f1)
            echo "#define $key 1"
            continue
        fi

        # Handle CONFIG_*=n  -> /* CONFIG_* is not set */
        if echo "$line" | grep -qE '^CONFIG_[A-Za-z0-9_]+=n$'; then
            key=$(echo "$line" | cut -d= -f1)
            echo "/* $key is not set */"
            continue
        fi

        # Handle "# CONFIG_* is not set" -> /* CONFIG_* is not set */
        if echo "$line" | grep -qE '^# CONFIG_[A-Za-z0-9_]+ is not set'; then
            key=$(echo "$line" | sed 's/^# \(CONFIG_[A-Za-z0-9_]*\) is not set/\1/')
            echo "/* $key is not set */"
            continue
        fi

        # Handle CONFIG_*="string" -> #define CONFIG_* "string"
        if echo "$line" | grep -qE '^CONFIG_[A-Za-z0-9_]+=".+"$'; then
            key=$(echo "$line" | cut -d= -f1)
            val=$(echo "$line" | cut -d= -f2-)
            echo "#define $key $val"
            continue
        fi

        # Handle CONFIG_*="" (empty string) -> #define CONFIG_* ""
        if echo "$line" | grep -qE '^CONFIG_[A-Za-z0-9_]+=""$'; then
            key=$(echo "$line" | cut -d= -f1)
            echo "#define $key \"\""
            continue
        fi

        # Handle CONFIG_*=number -> #define CONFIG_* number
        if echo "$line" | grep -qE '^CONFIG_[A-Za-z0-9_]+=[0-9]+$'; then
            key=$(echo "$line" | cut -d= -f1)
            val=$(echo "$line" | cut -d= -f2-)
            echo "#define $key $val"
            continue
        fi

    done < "$CONFIG_FILE"

    echo ""
    echo "#endif /* __AUTOCONF_H__ */"
} > "$AUTO_HEADER"

echo "  GEN     $AUTO_HEADER"
