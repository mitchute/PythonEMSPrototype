FakeEnergyPlus API
==================

As a part of this plugin work, we had to be able to provide plugins with access to some "built-in" functions.  These
functions include the functions for calculating thermophysical properties of fluids and refrigerants, along with some
other simulation mechanisms, like being able to issue a fatal error message.  The list of functions will be familiar to
users of the original EMS implementation, as these functions were exposed to EMS programs as built-in functions.

Implementation
--------------

As a quick overview of how this is implemented, I'll just run through the steps:

- A new set of functions are defined in a header file, within an `extern "C"` wrapper.
- The functions are also defined in an implementation file, again within an `extern "C"` wrapper.
- This API code is built into a standalone dynamic/shared library
- The core FakeEnergyPlus executable is then dynamically linked to this library
- In the Python interface class, during instantiation, the dynamic library is loaded in memory, and the functions are
  exposed, manually assigning the input/output types of the function, using the Python ctypes library
- In this way, any user defined derived class can leverage these pre-built function instances as self.api.functionXY()
