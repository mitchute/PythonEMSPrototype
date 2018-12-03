#!/bin/bash -e

# build the tool
mkdir build
cd build
cmake .. -G "Visual Studio 15 Win64"
cmake --build .
cd ..

# test out the built binary
cp /C/Python37/python37.dll build/Debug
PYTHONPATH=my_py_ems ./build/Debug/EnergyPlusPyEMS

# prep for packaging
mkdir tmp_build
cp build/Debug/EnergyPlusPyEMS.exe tmp_build/
cp /C/Python37/python37.dll tmp_build/
cp -r my_py_ems tmp_build/
cp scripts/launch.sh tmp_build/

# create the final package
mkdir release
/C/Program\ Files/7-zip/7z.exe -a release/PyEMSPrototype_Win_Debug.zip ./tmp_build/*
tar -zcvf release/PyEMSPrototype_Win_Debug.tar.gz -C tmp_build my_py_ems EnergyPlusPyEMS launch.sh python37.dll
