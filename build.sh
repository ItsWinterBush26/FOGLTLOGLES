OLD="$PWD"

rm -rf build
mkdir build
cd build

export ANDROID_NDK_ROOT="$ANDROID_SDK_ROOT/ndk-bundle"
export ANDROID_NDK="$ANDROID_NDK_ROOT"

cmake -G Ninja .. \
    -DCMAKE_TOOLCHAIN_FILE="$ANDROID_NDK/build/cmake/android.toolchain.cmake" \
    -DANDROID_ABI=armeabi-v7a \
    -DANDROID_PLATFORM=android-21 \
    -DCMAKE_BUILD_TYPE=Release

cmake --build .

cd $OLD