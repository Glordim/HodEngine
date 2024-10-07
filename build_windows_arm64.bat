@echo off
cmake -Bbuild_windows_arm64 -A ARM64

if %ERRORLEVEL% NEQ 0 (
    echo CMake configuration failed. Exiting...
    exit /b %ERRORLEVEL%
)

cmake --build build_windows_arm64 --config Debug
if %ERRORLEVEL% NEQ 0 (
    echo Debug build failed. Exiting...
    exit /b %ERRORLEVEL%
)

cmake --build build_windows_arm64 --config Release
if %ERRORLEVEL% NEQ 0 (
    echo Release build failed. Exiting...
    exit /b %ERRORLEVEL%
)

echo All builds completed successfully.