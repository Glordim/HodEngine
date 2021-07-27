@echo off

echo ====================

:: Update Submodules
echo Update Submodules...

git submodule update --init
if %errorlevel% neq 0 (
	echo Fail
	pause
	exit 1
)
echo Done

echo ====================

::Generate projects (using CMake)
echo Generate projects with CMake...

if not exist "./Build" (
	echo Create Build Dir
	mkdir "./Build"
)

echo *******
echo SDL...
if not exist "./Build/SDL" (
	echo Create SDL Build Dir
	mkdir "./Build/SDL"
)
cmake -A x64 -B./Build/SDL ./SDL
if %errorlevel% neq 0 (
	echo Fail
	pause
	exit 1
)
echo Done
echo *******
echo SPIRV-Cross...
if not exist "./Build/SPIRV-Cross" (
	echo Create SPIRV-Cross Build Dir
	mkdir "./Build/SPIRV-Cross"
)
cmake -A x64 -B./Build/SPIRV-Cross ./SPIRV-Cross -DSPIRV_CROSS_CLI=OFF -DSPIRV_CROSS_ENABLE_TESTS=OFF
if %errorlevel% neq 0 (
	echo Fail
	pause
	exit 1
)
echo Done
echo *******
echo box2d...
if not exist "./Build/box2d" (
	echo Create box2d Build Dir
	mkdir "./Build/box2d"
)
cmake -A x64 -B./Build/box2d ./box2d -DBOX2D_BUILD_UNIT_TESTS=OFF -DBOX2D_BUILD_TESTBED=OFF
if %errorlevel% neq 0 (
	echo Fail
	pause
	exit 1
)
echo Done
echo *******

echo ====================

::Build projects
echo Build projects...

echo *******
echo SDL...
cmake --build ./Build/SDL --config Release
if %errorlevel% neq 0 (
	echo Fail
	pause
	exit 1
)
echo Done
echo *******
echo SPIRV-Cross...
cmake --build ./Build/SPIRV-Cross --config Release
if %errorlevel% neq 0 (
	echo Fail
	pause
	exit 1
)
echo Done
echo *******
echo box2d...
cmake --build ./Build/box2d --config Release
if %errorlevel% neq 0 (
	echo Fail
	pause
	exit 1
)
echo *******

echo ====================
pause
exit 0
