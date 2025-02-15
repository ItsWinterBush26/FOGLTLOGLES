#!/bin/bash
set -e

BUILD_TYPE="android"
NDK_ROOT="${ANDROID_NDK_ROOT:-}"
ANDROID_ARCH="arm"
BUILD_DIR="build"
EXTRA_ARGS=()

while [[ $# -gt 0 ]]; do
    case "$1" in
        --ndk-root)
            NDK_ROOT="$2"
            shift 2
            ;;
        --arch)
            ANDROID_ARCH="$2"
            shift 2
            ;;
        --build-dir)
            BUILD_DIR="$2"
            shift 2
            ;;
        *)
            EXTRA_ARGS+=("$1")
            shift
            ;;
    esac
done

if [[ -z "$NDK_ROOT" ]]; then
    echo "Error: NDK root not specified. Use --ndk-root or set ANDROID_NDK_ROOT"
    exit 1
fi

if [[ ! -d "$NDK_ROOT" ]]; then
    echo "Error: NDK root directory does not exist: $NDK_ROOT"
    exit 1
fi

mkdir -p "$BUILD_DIR"

meson setup "$BUILD_DIR" \
    -Dndk_root="$NDK_ROOT" \
    -Dandroid_arch="$ANDROID_ARCH" \
    "${EXTRA_ARGS[@]}"

ninja -C "$BUILD_DIR"

echo "Build completed successfully for Android ($ANDROID_ARCH)"
