# PythonEMSPrototype

This is a little standalone sandbox where we can evaluate the design of the new Python-EMS implementation.

## Questions to answer with this repo

 - [ ] What version of Python will we use?
 - [ ] How is Python Packaged?
 - [ ] What does the Python API look like?
 - [ ] Will I write functions or classes?
 - [ ] Can I have more than one EMS file?
 - [ ] Can I have more than one function in a single file?
 - [ ] Can I write code that depends on other libraries?  If so, how?
 - [ ] What libraries will be built-in to the package?

## Developer Environment

I have tested it on Ubuntu 18.04, but it should be very close to functional on all the platforms.
Setting up a dev environment is really simple; you only need three things:

 - Python (currently targeting 3.6)
 - A C++ compiler (currently using g++)
 - CMake (to build the makefile structure -- I guess technically it's optional if you're a crazy person)
