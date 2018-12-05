#!/bin/bash -e

# build the tool
mkdir build
cd build
cmake .. -G "Visual Studio 15 Win64"
cmake --build . --config Release
cd ..

# test out the built binary
cp /C/Python37/python37.dll build/Release
echo " **** : Running the tool from the build directory"
./build/Release/EnergyPlusPyEMS example/in.idf

# grab and extract the embeddable python to get a zip copy of the standard library
mkdir python_embed
cd python_embed
curl https://www.python.org/ftp/python/3.7.1/python-3.7.1-embed-amd64.zip --output python_embed.zip
/c/Program\ Files/7-Zip/7z.exe x python_embed.zip
cd ..

# prep for packaging
mkdir tmp_build
cp build/Release/EnergyPlusPyEMS.exe tmp_build/
cp /C/Python37/python37.dll tmp_build/
cp /C/Python37/python.exe tmp_build/
cp python_embed/python37.zip tmp_build/
cp -r example tmp_build/
cp -r pyms/pyms tmp_build/

# create the final package
mkdir release
/C/Program\ Files/7-zip/7z.exe a release/PyEMSPrototype_Win.zip ./tmp_build/*
