#!/bin/bash -e

# build the tool
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
cmake --build . --config Release
cd ..

# test out the built binary
cp /C/Python37/python37.dll build/
PYTHONPATH=my_py_ems ./build/EnergyPlusPyEMS

# prep for packaging
mkdir tmp_build
cp build/EnergyPlusPyEMS.exe tmp_build/
cp /C/Python37/python37.dll tmp_build/
cp -r my_py_ems tmp_build/
cp scripts/launch.sh tmp_build/

# create the final package
mkdir release && tar -zcvf release/PyEMSPrototype_Win.tar.gz -C tmp_build my_py_ems EnergyPlusPyEMS launch.sh python37.dll
