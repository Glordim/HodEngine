@echo off
echo The Android NDK path is: %ANDROID_NDK%
echo Configure...
cmake -Bbuild_android_x64 -DCMAKE_TOOLCHAIN_FILE=%ANDROID_NDK%/build/cmake/android.toolchain.cmake -DANDROID_ABI=x86_64 -DANDROID_PLATFORM=android-33 -G "Ninja Multi-Config"

if %ERRORLEVEL% NEQ 0 (
    echo CMake configuration failed. Exiting...
    exit /b %ERRORLEVEL%
) else (
    echo Success
)

echo Build Debug...
cmake --build build_android_x64 --config Debug
if %ERRORLEVEL% NEQ 0 (
    echo Debug build failed. Exiting...
    exit /b %ERRORLEVEL%
) else (
    echo Success
)

echo Build Release...
cmake --build build_android_x64 --config Release
if %ERRORLEVEL% NEQ 0 (
    echo Release build failed. Exiting...
    exit /b %ERRORLEVEL%
) else (
    echo Success
)

echo All builds completed successfully.