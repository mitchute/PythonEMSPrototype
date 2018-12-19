#!/bin/bash -e

# build the tool
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j 2
cd ..
echo " **** : Running the tool from the build directory"
./build/FakeEnergyPlus example/in.idf

# prep for packaging
mkdir tmp_build
cp build/FakeEnergyPlus tmp_build/
cp build/libFakeAPI.so tmp_build/
cp -r example tmp_build/
cp -r plugin_interface/energyplus_plugin tmp_build/

# create final package
mkdir release
tar -zcvf release/FakeEnergyPlusWithEPS_Linux.tar.gz -C tmp_build example energyplus_plugin libFakeAPI.so FakeEnergyPlus
