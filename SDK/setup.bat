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
	echo Create SDK Build Dir
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
echo glm...
if not exist "./Build/glm" (
	echo Create glm Build Dir
	mkdir "./Build/glm"
)
cmake -A x64 -B./Build/glm ./glm
if %errorlevel% neq 0 (
	echo Fail
	pause
	exit 1
)
echo Done
echo *******
echo SPIRV-CROSS...
if not exist "./Build/SPIRV-CROSS" (
	echo Create SPIRV-CROSS Build Dir
	mkdir "./Build/SPIRV-CROSS"
)
cmake -A x64 -B./Build/SPIRV-CROSS ./SPIRV-CROSS
if %errorlevel% neq 0 (
	echo Fail
	pause
	exit 1
)
echo Done
echo *******
echo SDL...
if not exist "./Build/rapidjson" (
	echo Create rapidjson Build Dir
	mkdir "./Build/rapidjson"
)
cmake -A x64 -B./Build/rapidjson ./rapidjson
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
cmake -A x64 -B./Build/box2d ./box2d
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
cmake --build ./Build/SDL --config Debug
if %errorlevel% neq 0 (
	echo Fail
	pause
	exit 1
)
echo Done
echo *******
echo glm...
cmake --build ./Build/glm --config Release
if %errorlevel% neq 0 (
	echo Fail
	pause
	exit 1
)
cmake --build ./Build/glm --config Debug
if %errorlevel% neq 0 (
	echo Fail
	pause
	exit 1
)
echo Done
echo *******
echo SPIRV-CROSS...
cmake --build ./Build/SPIRV-CROSS --config Release
if %errorlevel% neq 0 (
	echo Fail
	pause
	exit 1
)
cmake --build ./Build/SPIRV-CROSS --config Debug
if %errorlevel% neq 0 (
	echo Fail
	pause
	exit 1
)
echo Done
echo *******
echo rapidjson...
cmake --build ./Build/rapidjson --config Release
if %errorlevel% neq 0 (
	echo Fail
	pause
	exit 1
)
cmake --build ./Build/rapidjson --config Debug
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
cmake --build ./Build/box2d --config Debug
if %errorlevel% neq 0 (
	echo Fail
	pause
	exit 1
)
echo Done
echo *******

echo ====================
pause
exit 0
