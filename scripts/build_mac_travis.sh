#!/bin/bash -e

# build the tool
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DTRAVIS=Y ..
make
cd ..
echo " **** : Running the tool from the build directory"
PYTHONPATH=my_py_ems ./build/EnergyPlusPyEMS in.idf

# prep for packaging
mkdir tmp_build
cp build/EnergyPlusPyEMS tmp_build/
cp -r my_py_ems tmp_build/
cp scripts/launch.sh tmp_build/
cp in.idf tmp_build/

# collect up the Python framework that was used in this build
cp /usr/local/Frameworks/Python.framework/Versions/3.6/Python tmp_build/libpython3.6.dylib
# we don't necessary _need_ python3.6 binary to do anything, it's just another interpreter
# but it could be nice to at least keep it around for testing if nothing else
# if we remove it here, don't forget to clean up the install_name_tool commands below as well
cp /usr/local/Frameworks/Python.framework/Versions/3.6/Resources/Python.app/Contents/MacOS/Python tmp_build/python3.6
cp -r /usr/local/Frameworks/Python.framework/Versions/3.6/lib tmp_build/
# clean out some stuff we don't actually need - this list might get longer
rm -f tmp_build/lib/libpython3.6.dylib
rm -f tmp_build/lib/libpython3.6m.dylib
rm -r tmp_build/lib/pkgconfig

# we'll want to make some modifications to the DLL reference paths, need to start by adjusting permissions on the Python lib itself
chmod 777 tmp_build/libpython3.6.dylib
# this ID command is an interesting one, but it seems to be necessary, followed by some regular DLL reference changes
install_name_tool -id @executable_path/libpython3.6.dylib tmp_build/libpython3.6.dylib
# otool -L tmp_build/python3.6 - Diagnostic
install_name_tool -change /usr/local/Cellar/python/3.6.5_1/Frameworks/Python.framework/Versions/3.6/Python @executable_path/libpython3.6.dylib tmp_build/python3.6
# otool -L tmp_build/python3.6 - Diagnostic
install_name_tool -change /usr/local/opt/python/Frameworks/Python.framework/Versions/3.6/Python @executable_path/libpython3.6.dylib tmp_build/EnergyPlusPyEMS

# calling this here ensures that the Python prep we did in the tmp_build dir works
cd tmp_build
echo " **** : Launching the tool from the packaging directory"
./launch.sh
cd ..

# create the final package
mkdir release && tar -zcf release/PyEMSPrototype_Mac.tar.gz -C tmp_build my_py_ems EnergyPlusPyEMS launch.sh lib libpython3.6.dylib python3.6 in.idf
