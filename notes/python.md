# Python Dependency

So, this is a C++ application that calls out to the Python shared library.
What we want to do is to ensure that when a user downloads the binary to an acceptable machine, it runs without missing any dependencies.
Each platform requires a bit different treatment.

## Linux

On Linux, we are focusing on Ubuntu 18.04 for right now.
The binary is built on Travis, with an 18.04 Docker image, and at the time of the writing has a list of 12 shared object dependencies.
Each has been investigated to determine whether it is something we need to collect for binary distribution or not.
Here are the results:

- linux-vdso.so.1 (0x00007ffd991d8000)
    - This is the Elf virtual Dynamic Shared Object, and is used as a system call interface between user land and kernel land to avoid expensive context switching
- libstdc++.so.6 => /usr/lib/x86_64-linux-gnu/libstdc++.so.6 (0x00007fcfea797000)
    - This is the GNU standard C++ library
    - This, along with libgcc and libc are the reason we aren't compatible on older versions of Ubuntu
- libgcc_s.so.1 => /lib/x86_64-linux-gnu/libgcc_s.so.1 (0x00007fcfea57f000)
    - This is the GCC support library
- libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007fcfea18e000)
    - This is the GNU standard C library
- libexpat.so.1 => /lib/x86_64-linux-gnu/libexpat.so.1 (0x00007fcfe9f5c000)
    - This is the XML parsing C library
    - According to the Ubuntu 18.04 Desktop package manifest file (http://releases.ubuntu.com/18.04/ubuntu-18.04.1-desktop-amd64.manifest), this is preinstalled, so we're good here
- libz.so.1 => /lib/x86_64-linux-gnu/libz.so.1 (0x00007fcfe9d3f000)
    - According to dpkg (dpkg -S /lib/x86_64-linux-gnu/libz.so.1), this is installed via the apt package `zlib1g`
    - According to the Ubuntu 18.04 manifest, this is also a requirement for Ubuntu
- libpthread.so.0 => /lib/x86_64-linux-gnu/libpthread.so.0 (0x00007fcfe9b20000)
- libdl.so.2 => /lib/x86_64-linux-gnu/libdl.so.2 (0x00007fcfe991c000)
- libutil.so.1 => /lib/x86_64-linux-gnu/libutil.so.1 (0x00007fcfe9719000)
- libm.so.6 => /lib/x86_64-linux-gnu/libm.so.6 (0x00007fcfe937b000)
- /lib64/ld-linux-x86-64.so.2 (0x00007fcfeb3e7000)
    - According to dpkg, all 5 of these come with libc6 - good
- libpython3.6m.so.1.0 => /usr/lib/x86_64-linux-gnu/libpython3.6m.so.1.0 (0x00007fcfeab20000)
    - This is the main python library
    - This is a bit tricky.  According to the 18.04 manifest, libpython3.6 is a required package, so 3.6 will always be available.
    - However, if Ubuntu does an upgrade and the ABI (not API) changes, will this still work?  I think it will if we compile against the minimal_api.
    - We could consider grabbing this from the build machine and dropping it right next to the binary, then either:
        - Use patchelf to force the binary to look to our library, or
        - Use LD_LIBRARY_PATH=. on the command line to have it find this one first
    - Also, we could potentially just have it next to our binary, and if the system doesn't find one pre-installed it would use ours?

Ultimately, the only one we really care about is the libpython3.6 library.
My current feeling is that we don't worry about including our own version.
This has the advantage of ensuring that the EMS dev can use system-native dependencies by installing them using system apt python packages.
When you report the sys.path from the EMS code, you get the paths:
    ['my_py_ems', '/usr/lib/python36.zip', '/usr/lib/python3.6', '/usr/lib/python3.6', '/usr/lib/python3.6/lib-dynload']
So the dev can drop their own libraries into my_py_ems, and also utilize system-installed libraries, side-by-side.
We'll need to evaluate how this jives with other platforms as we tidy all this up, but for now we will rely on the Ubuntu supplied Python lib, and not package one ourselves.

## Mac

So with Mac, I think it's going to be best to package up our own Python library.
We don't necessarily need our own interpreter, though it could be nice for a user to be able to run it to ensure they have things set up correctly for E+.  Maybe.

Python doesn't make pre-packaged self-contained Python for Mac, but it's pretty easy to make our own.
There were multiple random Stack Overflow questions and other sources of information, but the best by far was from http://joaoventura.net/blog/2016/embeddable-python-osx/
The Mac Python build installed from HomeBrew is actually pretty self-contained, but it does need a little massaging to ensure our packed version is fully portable.
This was verified by testing the Travis-built-Python-3.6 package on a Mac which only contained Python 3.7.

- Let's set up a temporary directory to house our portable Python pack:
    - mkdir tmp_build
- We need to get the Python shared library itself.  This is what the C++ code links against (it's not the Python standard library).  Note we give it a meaningful name with version:
    - cp /usr/local/Frameworks/Python.framework/Versions/3.6/Python tmp_build/libpython3.6.dylib
- We don't necessary _need_ the python3.6 binary for our purposes.  Our C++ code essentially replaces it.  However, users could potentially use it to test environments/packages, so we'll bring it along:
    - cp /usr/local/Frameworks/Python.framework/Versions/3.6/Resources/Python.app/Contents/MacOS/Python tmp_build/python3.6
- We need to also get the actual Python standard library (the stuff that provides `import json` in your Python code):
    - cp -r /usr/local/Frameworks/Python.framework/Versions/3.6/lib tmp_build/
- But we don't need everything, so we can clean out a little as well:
    - rm -f tmp_build/lib/libpython3.6.dylib
    - rm -f tmp_build/lib/libpython3.6m.dylib
    - rm -r tmp_build/lib/pkgconfig
- Now we have all the files, but they will still try to find the system installed Python, so we need to adjust that.  To start we need to adjust permissions on the Python lib itself to allow it to be rewritten:
    - chmod 777 tmp_build/libpython3.6.dylib
- Next we change the "ID" on the Python shared library.  I'm still a little unsure about the reasoning, but it works:
    - install_name_tool -id @executable_path/libpython3.6.dylib tmp_build/libpython3.6.dylib
- Now we just need to tell our built program binary to look for the Python shared library real close by:
    - install_name_tool -change /usr/local/opt/python/Frameworks/Python.framework/Versions/3.6/Python @executable_path/libpython3.6.dylib tmp_build/EnergyPlusPyEMS
- And if we are including the interpreter, we need to do the same for it:
    - install_name_tool -change /usr/local/Cellar/python/3.6.5_1/Frameworks/Python.framework/Versions/3.6/Python @executable_path/libpython3.6.dylib tmp_build/python3.6

## Windows

On Windows, the only thing we are currently doing is bringing along the python37 DLL file from the installed version.
Using Dependency Walker, it looks like this depends on core Windows libraries, plus the visual c++ runtime.