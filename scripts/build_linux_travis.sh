#!/bin/bash -e

# build the tool
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j 2
cd ..
echo " **** : Running the tool from the build directory"
./build/EnergyPlusPyEMS example/in.idf

# prep for packaging
mkdir tmp_build
cp build/EnergyPlusPyEMS tmp_build/
cp -r example tmp_build/
cp -r pyms/pyms tmp_build/

# create final package
mkdir release
tar -zcvf release/PyEMSPrototype_Linux.tar.gz -C tmp_build example pyms EnergyPlusPyEMS
