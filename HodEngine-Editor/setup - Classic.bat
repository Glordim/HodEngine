@echo off

if not exist "./Build" (
	echo Create Build Dir
	mkdir "./Build"
)

cmake -A x64 -B ./Build
if %errorlevel% neq 0 (
	echo Fail
	pause
	exit 1
)
echo Done

exit 0
