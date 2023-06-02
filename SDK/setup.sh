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
echo "SPIRV-Cross..."
if [ ! -d "./Build/SPIRV-Cross" ]; then
	echo "Create SPIRV-CROSS Build Dir"
	mkdir "./Build/SPIRV-Cross"
fi
cmake -B ./Build/SPIRV-Cross ./SPIRV-Cross -DSPIRV_CROSS_CLI=OFF -DSPIRV_CROSS_ENABLE_TESTS=OFF
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
cmake -B ./Build/box2d ./box2d -DBOX2D_BUILD_UNIT_TESTS=OFF -DBOX2D_BUILD_TESTBED=OFF
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
echo "SPIRV-Cross..."
cmake --build ./Build/SPIRV-Cross --config Release
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
echo "Done"
echo "*******"

echo "===================="
exit 0
