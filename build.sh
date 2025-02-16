OLD="$PWD"

set -e

sudo apt update
sudo apt install ninja-build bison -y

if [[ -z "$ANDROID_SDK_ROOT" ]]; then
    echo "Make sure '\$ANDROID_SDK_ROOT' is set before building!"
    exit 1
fi

if [[ -z "$ANDROID_NDK_ROOT" ]]; then
    ndk_ver_dir=$(find "$ANDROID_SDK_ROOT/ndk" -maxdepth 1 -type d -printf "%f\n" \
        | grep -E '^[0-9]+\.[0-9]+\.[0-9]+$' \
        | sort -V \
        | tail -n 1)

    if [[ -z "$ANDROID_SDK_ROOT/ndk/$ndk_ver_dir" ]]; then
        echo "No NDKs found in '$ANDROID_SDK_ROOT/$ndk_ver_dir'."
        echo "Trying 'ndk-bundle'"
        ndk_ver_dir="ndk-bundle"
        if [[ ! -d "$ANDROID_SDK_ROOT/$ndk_ver_dir" ]]; then
            echo "Failed to find a suitable NDK!"
            exit 1
        else
            echo "Using 'ndk-bundle' at '$ANDROID_SDK_ROOT/$ndk_ver_dir'"
            export ANDROID_NDK_ROOT="$ANDROID_SDK_ROOT/$ndk_ver_dir"
        fi
    else
        echo "Using NDK at '$ANDROID_SDK_ROOT/$ndk_ver_dir'"
        export ANDROID_NDK_ROOT="$ANDROID_SDK_ROOT/ndk/$ndk_ver_dir"
    fi
else
    echo "Using NDK at '$ANDROID_NDK_ROOT'"
fi

./deps/shaderc/utils/git-sync-deps

rm -rf build
mkdir build
cd build

echo "Building for '$1'"
export NDK_CCACHE="$(which ccache)"
export ANDROID_CCACHE="$NDK_CCACHE"
cmake -G Ninja .. \
    -DCMAKE_TOOLCHAIN_FILE="$ANDROID_NDK_ROOT/build/cmake/android.toolchain.cmake" \
    -DANDROID_ABI=$1 \
    -DANDROID_PLATFORM=24 \
    -DANDROID_CCACHE="$NDK_CCACHE" \
    -DNDK_CCACHE="$NDK_CCACHE" \
    -DCMAKE_BUILD_TYPE=Release

CMAKE_BUILD_PARALLEL_LEVEL="$(nproc)"
cmake --build .

cd "$OLD"
