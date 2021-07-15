#!/bin/sh

echo "===================="

# Update Submodules
echo "Update Submodules..."

git submodule update --init
if [ $? -ne 0 ]; then
	echo "Fail"
	exit 1
fi
echo "Done"

echo "===================="

# Generate projects (using CMake)
echo "Generate projects with CMake..."

if [ ! -d "./Build" ]; then
	echo "Create Build Dir"
	mkdir "./Build"
fi

echo "*******"
echo "SDL..."
if [ ! -d "./Build/SDL" ]; then
	echo "Create SDL Build Dir"
	mkdir "./Build/SDL"
fi
cmake -B./Build/SDL ./SDL
if [ $? -ne 0 ]; then
	echo "Fail"
	exit 1
fi
echo "Done"
echo "*******"
echo "glm..."
if [ ! -d "./Build/glm" ]; then
	echo "Create glm Build Dir"
	mkdir "./Build/glm"
fi
cmake -B./Build/glm ./glm
if [ $? -ne 0 ]; then
	echo "Fail"
	exit 1
fi
echo "Done"
echo "*******"
echo "SPIRV-Cross..."
if [ ! -d "./Build/SPIRV-Cross" ]; then
	echo "Create SPIRV-CROSS Build Dir"
	mkdir "./Build/SPIRV-Cross"
fi
cmake -B./Build/SPIRV-Cross ./SPIRV-Cross
if [ $? -ne 0 ]; then
	echo "Fail"
	exit 1
fi
echo "Done"
echo "*******"
echo "SDL..."
if [ ! -d "./Build/rapidjson" ]; then
	echo "Create rapidjson Build Dir"
	mkdir "./Build/rapidjson"
fi
cmake -B./Build/rapidjson ./rapidjson
if [ $? -ne 0 ]; then
	echo "Fail"
	exit 1
fi
echo "Done"
echo "*******"
echo "box2d..."
if [ ! -d "./Build/box2d" ]; then
	echo "Create box2d Build Dir"
	mkdir "./Build/box2d"
fi
cmake -B./Build/box2d ./box2d
if [ $? -ne 0 ]; then
	echo "Fail"
	exit 1
fi
echo "Done"
echo "*******"

echo "===================="

#Build projects
echo "Build projects..."

echo "*******"
echo "SDL..."
#cmake --build ./Build/SDL --config Release
if [ $? -ne 0 ]; then
	echo "Fail"
	exit 1
fi
#cmake --build ./Build/SDL --config Debug
if [ $? -ne 0 ]; then
	echo "Fail"
	exit 1
fi
echo "Done"
echo "*******"
echo "glm..."
#cmake --build ./Build/glm --config Release
if [ $? -ne 0 ]; then
	echo "Fail"
	exit 1
fi
#cmake --build ./Build/glm --config Debug
if [ $? -ne 0 ]; then
	echo "Fail"
	exit 1
fi
echo "Done"
echo "*******"
echo "SPIRV-Cross..."
cmake --build ./Build/SPIRV-Cross --config Release
if [ $? -ne 0 ]; then
	echo "Fail"
	exit 1
fi
cmake --build ./Build/SPIRV-Cross --config Debug
if [ $? -ne 0 ]; then
	echo "Fail"
	exit 1
fi
echo "Done"
echo "*******"
echo "rapidjson..."
cmake --build ./Build/rapidjson --config Release
if [ $? -ne 0 ]; then
	echo "Fail"
	exit 1
fi
cmake --build ./Build/rapidjson --config Debug
if [ $? -ne 0 ]; then
	echo "Fail"
	exit 1
fi
echo "Done"
echo "*******"
echo "box2d..."
cmake --build ./Build/box2d --config Release
if [ $? -ne 0 ]; then
	echo "Fail"
	exit 1
fi
cmake --build ./Build/box2d --config Debug
if [ $? -ne 0 ]; then
	echo "Fail"
	exit 1
fi
echo "Done"
echo "*******"

echo "===================="
exit 0
