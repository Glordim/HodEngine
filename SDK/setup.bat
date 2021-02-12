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
echo tinyobjloader...
if not exist "./Build/tinyobjloader" (
	echo Create tinyobjloader Build Dir
	mkdir "./Build/tinyobjloader"
)
cmake -A x64 -B./Build/tinyobjloader ./tinyobjloader
if %errorlevel% neq 0 (
	echo Fail
	pause
	exit 1
)
echo Done
echo *******
echo PhysX...

call PhysX\physx\generate_projects.bat vc19win64
if %errorlevel% neq 0 (
	echo Fail
	pause
	exit 1
)

if not exist "./Build/PhysX_proj" (
	echo Create SymLink to PhysX_proj Dir
	mklink /J "%CD%\Build\PhysX_proj" "%CD%\PhysX\physx\compiler\vc19win64"
)
if not exist "./Build/PhysX_lib" (
	echo Create SymLink to PhysX_lib Dir
	mklink /J "%CD%\Build\PhysX_lib" "%CD%\PhysX\physx\bin\win.x86_64.vc141.md"
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
echo tinyobjloader...
cmake --build ./Build/tinyobjloader --config Release
if %errorlevel% neq 0 (
	echo Fail
	pause
	exit 1
)
cmake --build ./Build/tinyobjloader --config Debug
if %errorlevel% neq 0 (
	echo Fail
	pause
	exit 1
)
echo Done
echo *******
echo PhysX...
cmake --build ./Build/PhysX_proj --config Release
cmake --build ./Build/PhysX_proj --config Debug

echo Done
echo *******

echo ====================
pause
exit 0
