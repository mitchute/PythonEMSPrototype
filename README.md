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
 - [ ] What does the Python API look like?
 - [ ] Will I write functions or classes?
 - [ ] Can I have more than one EMS file?
 - [ ] Can I have more than one function in a single file?
 - [ ] Can I write code that depends on other libraries?  If so, how?
 - [ ] What libraries will be built-in to the package?

## Developer Environment

Setting up a dev environment is really simple; you only need three things:

 - Python (currently targeting 3.6)
 - A C++ compiler (currently using g++ on Linux, Clang on Mac, and MSVC on Windows)
 - CMake

## Python

Tons of notes about Python interactions, packaging, etc., can be found in the [Python notes file](notes/python.md).
