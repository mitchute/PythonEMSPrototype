Background & Justification
==========================

EnergyPlus is a whole building energy simulation tool, written mostly in C++, with some auxiliary tools in other
languages such as Fortran and Python.  The bulk of the heavy lifting is done in the main EnergyPlus binary, which
contains the instructions necessary to simulate predetermined thermophysical phenomena of buildings along with a
predetermined set of control strategies.

In 2006, a custom runtime language was added to the EnergyPlus input syntax, with a custom built parser and interpreter.
This system is called EnergyPlus Energy Management System, or EMS for short, with the language being the EnergyPlus
Runtime Language, or Erl for short.  This system could be used by program users who wanted to customize control
strategies or override some aspects of the program.  The code could "sense" conditions of the simulation and
"actuate" output variables.

Over time, the implementation extended to allowing users to create their own custom "components", where the EMS was
invoked to calculate physical responses of components to various inlet conditions, environmental conditions, and
simulation status such as time of day.  In conjunction with this extension, the bulkiness of a built-in interpretive
language began revealing itself.  Debugging large programs became increasingly difficult for users, and managing a
"library" of EMS was very difficult.

This new implementation of EMS in Python will address many of the issues users see with the current Erl-based EMS
system.  To avoid ambiguity, the new implementation will, for now, be referred to as EnergyPlus PyMS.  The key
requirements of the new system include:

* Users must be able to write Python functions that can be callable from inside EnergyPlus
* PyMS functions must have access to sensors in EnergyPlus, and must be able to actuate exposed components in EnergyPlus
* Users do not need to have any version of Python installed (unless it is a core (required) asset in the operating system)
* The placement of PyMS script files should be flexible, with little limitation on how the user organizes their files
* PyMS should allow the user to build up a library of scripts and functionality, with multiple classes per file, etc.
* Users should be able to add other libraries to be importable at runtime with the single caveat that libraries
  that use native code will not be explicitly supported, but may still be functional
