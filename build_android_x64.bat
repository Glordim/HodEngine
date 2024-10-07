@echo off
echo The Android NDK path is: %ANDROID_NDK%
cmake -Bbuild_android_x64 -DCMAKE_TOOLCHAIN_FILE=%ANDROID_NDK%/build/cmake/android.toolchain.cmake -DANDROID_ABI=x86_64 -DANDROID_PLATFORM=android-33 -G "Ninja Multi-Config"

if %ERRORLEVEL% NEQ 0 (
    echo CMake configuration failed. Exiting...
    exit /b %ERRORLEVEL%
)

cmake --build build_android_x64 --config Debug
if %ERRORLEVEL% NEQ 0 (
    echo Debug build failed. Exiting...
    exit /b %ERRORLEVEL%
)

cmake --build build_android_x64 --config Release
if %ERRORLEVEL% NEQ 0 (
    echo Release build failed. Exiting...
    exit /b %ERRORLEVEL%
)

echo All builds completed successfully.