# PythonEMSPrototype

[![Build Status](https://travis-ci.org/Myoldmopar/PythonEMSPrototype.svg?branch=master)](https://travis-ci.org/Myoldmopar/PythonEMSPrototype)

This is a little standalone sandbox where we can evaluate the design of the new Python-EMS implementation.

## Questions to answer with this repo

 - [X] What version of Python will we use?
   - The plan is to get all platforms using Python 3.6
 - [X] How is Python Packaged?
   - On Ubuntu 18.04, we are currently using the system installed Python 3.6
   - On Windows we are packaging an isolated Python 3.7 environment with a compressed standard library (issue #14)
   - On Mac we are packaging an isolated Python 3.6 environment with an uncompressed standard library
 - [X] What does the Python API look like?
   - It is a very simple interface with 4 required functions:
     - `ems_main`: the main worker function
     - `get_calling_point`: returns when this should be called by the EMS in E+
     - `get_sensed_data_list`: returns a list of the sensors used in this program
     - `get_actuator_list`: returns a list of the actuators this program will update
 - [X] Will I write functions or classes?
   - A very minimal class structure with four methods is required
 - [X] Can I have more than one EMS file?
   - Yes, for sure
 - [X] Can I have more than one function in a single file?
   - Yes, for sure, that's what the current example already does
 - [X] Can I write code that depends on other libraries?  If so, how?
   - Well, yes, and no.  You'll have the entire Python standard library, plus any additional libraries
     found in the search path.  The search path is essentially a single folder, the `my_py_ems` folder in this demo.
     In this example, I have made a subpackage that I import in my EMS code to demonstrate how you can drop in packages.
     All this said, dropping in packages that depend on native code (NumPy, etc.), is not going to be as
     straightforward, and we will not initially concern ourselves with it.
   - There is an aside here, and that is that on Linux, we are using the system Python 3.6.  So if packages are
     installed into that library, they could be available for your PyEMS.
 - [X] What libraries will be built-in to the package?
   - The entire Python 3.6 standard library.  Plus anything else you drop in the Python search path.

## Developer Environment

Setting up a dev environment is really simple; you only need three things:

 - Python (currently targeting 3.6)
 - A C++ compiler (currently using g++ on Linux, Clang on Mac, and MSVC on Windows)
 - CMake

## Python

Tons of notes about Python interactions, packaging, etc., can be found in the [Python notes file](notes/python.md).
