#!/bin/bash -e

# build the tool
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j 2
cd ..
echo " **** : Running the tool from the build directory"
PYTHONPATH=my_py_ems ./build/EnergyPlusPyEMS in.idf

# prep for packaging
mkdir tmp_build
cp build/EnergyPlusPyEMS tmp_build/
cp -r my_py_ems tmp_build/
cp scripts/launch.sh tmp_build/
cp in.idf tmp_build/

# create final package
mkdir release
tar -zcvf release/PyEMSPrototype_Linux.tar.gz -C tmp_build my_py_ems EnergyPlusPyEMS launch.sh in.idf
