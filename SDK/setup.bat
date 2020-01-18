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
cmake -B./Build/SDL ./SDL
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
cmake -B./Build/glm ./glm
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
cmake -B./Build/SPIRV-CROSS ./SPIRV-CROSS
if %errorlevel% neq 0 (
	echo Fail
	pause
	exit 1
)
echo Done
echo *******
echo tinyobjloader...
if not exist "./Build/tinyobjloader" (
	echo Create tinyobjloader Build Dir
	mkdir "./Build/tinyobjloader"
)
cmake -B./Build/tinyobjloader ./tinyobjloader
if %errorlevel% neq 0 (
	echo Fail
	pause
	exit 1
)
echo Done
echo *******
echo PhysX...

call PhysX\physx\generate_projects.bat vc15win64
if %errorlevel% neq 0 (
	echo Fail
	pause
	exit 1
)

if not exist "./Build/PhysX" (
	echo Create SymLink to PhysX Build Dir
	mklink /J /D %CD%\Build\PhysX %CD%\PhysX\physx\compiler\vc15win64
)
echo Done
echo *******

echo ====================
pause
exit 0
