#!/bin/bash

# ENI Mod Build Script
# Run this from the project root: ./build.sh

set -e

echo "=== ENI Mod Build ==="
echo "Building native library..."

# Build native .so using NDK
cd app/src/main/cpp
$ANDROID_NDK_HOME/ndk-build NDK_PROJECT_PATH=. APP_BUILD_SCRIPT=Android.mk

echo "=== Native build complete ==="
echo "Now build the APK in Android Studio or with:"
echo "./gradlew assembleDebug"
