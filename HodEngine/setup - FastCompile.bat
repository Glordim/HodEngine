@echo off

if not exist "./Build" (
	echo Create Build Dir
	mkdir "./Build"
)

cmake -A x64 -B ./Build -DCMAKE_UNITY_BUILD=ON -DCMAKE_UNITY_BUILD_BATCH_SIZE=16
if %errorlevel% neq 0 (
	echo Fail
	pause
	exit 1
)
echo Done

exit 0
