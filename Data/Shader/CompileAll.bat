:: Reset errorlevel status so we are not inheriting this state from the calling process:
@call :CLEAN_EXIT
echo off

set GLSL_VALIDATOR=C:\VulkanSDK\1.1.114.0\Bin\glslangValidator.exe

for %%f in (*.vert) do (echo %%f & %GLSL_VALIDATOR% -Od --target-env vulkan1.1 -o %%f.spirv %%f)

for %%f in (*.frag) do (echo %%f & %GLSL_VALIDATOR% -Od --target-env vulkan1.1 -o %%f.spirv %%f)

pause


:CLEAN_EXIT
exit /b 0