#!/usr/bin/env bash

set -euo pipefail

normalize_bool() {
    case "${1:-}" in
        1|ON|on|TRUE|true|YES|yes) return 0 ;;
        *) return 1 ;;
    esac
}

CEF_VERSION="${LAUNCHER_CEF_VERSION:-${CEF_VERSION:-}}"
CEF_DISTRIBUTION="${LAUNCHER_CEF_DISTRIBUTION:-${CEF_DISTRIBUTION:-minimal}}"
CEF_BRANCH="${LAUNCHER_CEF_SOURCE_BRANCH:-${CEF_SOURCE_BRANCH:-}}"
CEF_DOWNLOAD_ROOT="${LAUNCHER_CEF_SOURCE_DOWNLOAD_DIR:-${CEF_SOURCE_DOWNLOAD_DIR:-${PWD}/.cache/cef-source}}"
CEF_DEPOT_TOOLS_DIR="${LAUNCHER_CEF_DEPOT_TOOLS_DIR:-${CEF_DEPOT_TOOLS_DIR:-${CEF_DOWNLOAD_ROOT}/depot_tools}}"
CEF_OUT_FILE="${LAUNCHER_CEF_SOURCE_OUT_FILE:-}"
CEF_INSTALL_BUILD_DEPS="${LAUNCHER_CEF_INSTALL_BUILD_DEPS:-0}"
CEF_FORCE_CLEAN="${LAUNCHER_CEF_SOURCE_FORCE_CLEAN:-0}"
CEF_GN_DEFINES="${LAUNCHER_CEF_GN_DEFINES:-${CEF_GN_DEFINES:-is_official_build=true use_sysroot=true symbol_level=0}}"

if [[ -z "${CEF_BRANCH}" && -n "${CEF_VERSION}" && "${CEF_VERSION}" =~ chromium-[0-9]+\.0\.([0-9]+)\.[0-9]+ ]]; then
    CEF_BRANCH="${BASH_REMATCH[1]}"
fi

if [[ -z "${CEF_BRANCH}" ]]; then
    echo "LAUNCHER_CEF_SOURCE_BRANCH or CEF_SOURCE_BRANCH must be set for CEF source builds." >&2
    exit 1
fi

DOWNLOAD_DIR="${CEF_DOWNLOAD_ROOT}/chromium_git"
TOOLS_DIR="${CEF_DOWNLOAD_ROOT}/cef-tools"
AUTOMATE_SCRIPT="${TOOLS_DIR}/tools/automate/automate-git.py"
DISTRIB_PARENT="${DOWNLOAD_DIR}/chromium/src/cef/binary_distrib"

mkdir -p "${CEF_DOWNLOAD_ROOT}"

if [[ ! -d "${TOOLS_DIR}/.git" ]]; then
    git clone --filter=blob:none --depth 1 https://github.com/chromiumembedded/cef.git "${TOOLS_DIR}"
else
    git -C "${TOOLS_DIR}" fetch --depth 1 origin HEAD
    git -C "${TOOLS_DIR}" reset --hard FETCH_HEAD
fi

if normalize_bool "${CEF_FORCE_CLEAN}"; then
    rm -rf "${DISTRIB_PARENT}"
fi

find_existing_distrib() {
    local pattern="cef_binary_*_linux64*"
    if [[ -n "${CEF_VERSION}" ]]; then
        pattern="cef_binary_${CEF_VERSION}_linux64*"
    fi
    find "${DISTRIB_PARENT}" -maxdepth 1 -mindepth 1 -type d -name "${pattern}" | sort | tail -n 1
}

CEF_DISTRIB_ROOT="$(find_existing_distrib || true)"
if [[ -n "${CEF_DISTRIB_ROOT}" && -f "${CEF_DISTRIB_ROOT}/cmake/FindCEF.cmake" && -f "${CEF_DISTRIB_ROOT}/Release/libcef.so" ]]; then
    echo "Reusing existing CEF binary distribution at ${CEF_DISTRIB_ROOT}"
else
    export GN_DEFINES="${CEF_GN_DEFINES}"
    export CEF_USE_GN=1

    base_args=(
        --download-dir="${DOWNLOAD_DIR}"
        --depot-tools-dir="${CEF_DEPOT_TOOLS_DIR}"
        --branch="${CEF_BRANCH}"
        --x64-build
    )

    if normalize_bool "${CEF_INSTALL_BUILD_DEPS}"; then
        python3 "${AUTOMATE_SCRIPT}" "${base_args[@]}" --no-build
        "${DOWNLOAD_DIR}/chromium/src/build/install-build-deps.sh" \
            --no-prompt \
            --no-arm \
            --no-chromeos-fonts
    fi

    build_args=(
        --minimal-distrib
        --minimal-distrib-only
        --no-debug-build
        --force-distrib
    )
    if normalize_bool "${CEF_FORCE_CLEAN}"; then
        build_args+=(
            --force-clean
            --force-update
            --force-build
        )
    fi

    python3 "${AUTOMATE_SCRIPT}" "${base_args[@]}" "${build_args[@]}"
    CEF_DISTRIB_ROOT="$(find_existing_distrib || true)"
fi

if [[ -z "${CEF_DISTRIB_ROOT}" || ! -f "${CEF_DISTRIB_ROOT}/cmake/FindCEF.cmake" || ! -f "${CEF_DISTRIB_ROOT}/Release/libcef.so" ]]; then
    echo "CEF source build did not produce a usable Linux binary distribution." >&2
    exit 1
fi

echo "Using CEF binary distribution from source build: ${CEF_DISTRIB_ROOT}"
if [[ -n "${CEF_OUT_FILE}" ]]; then
    printf '%s\n' "${CEF_DISTRIB_ROOT}" > "${CEF_OUT_FILE}"
fi
