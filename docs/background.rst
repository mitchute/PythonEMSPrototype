Background & Justification
==========================

EnergyPlus is a whole building energy simulation tool, written mostly in C++, with some auxiliary tools in other
languages such as Fortran and Python.  The bulk of the heavy lifting is done in the main EnergyPlus binary, which
contains the instructions necessary to simulate predetermined thermophysical phenomena of buildings along with a
predetermined set of control strategies.

In 2006, a custom runtime language was added to the EnergyPlus input syntax, with a custom built parser and interpreter.
This system is called EnergyPlus Energy Management System, or EMS for short, with the language being the EnergyPlus
Runtime Language, or Erl for short.  This system could be leveraged by program users who wanted to customize control
strategies or override some aspects of the program.  The code could "sense" conditions of the simulation and
"actuate" output variables.

Over time, the implementation extended to allowing users to create their own custom "components", where the EMS was
invoked to calculate physical responses of components to various inlet conditions, environmental conditions, and
simulation status such as time of day.  In conjunction with this extension, the bulkiness of a built-in interpretive
language began revealing itself.  Debugging large programs became increasingly difficult for users, and managing a
"library" of EMS was very difficult.  The breadth of capability with EMS has also outgrown the original name, as it is
no longer just an energy management system, but basically a plugin system for affecting the simulation with custom code.

A new system has now been developed, called the EnergyPlus Plugin System (or EPS for short).  This builds on the ideas
of EMS, but now allows users to write plugin code using the Python language, instead of the built-in runtime language.
This unlocks an incredible amount of possibilities, as now the user can write code that does pretty much anything Python
can do:

* Typical mathematical operations
* File I/O for accessing data on a system
* Web queries
* Hardware queries

This implementation of the plugin system in Python will address many of the issues users see with the current Erl-based
EMS system.  The key facets of the new system include:

* Users write Python functions that are callable from inside EnergyPlus
* Plugin functions have access to sensors in EnergyPlus, and are able to actuate exposed components in EnergyPlus
* Users on Windows and Mac do not need to have any version of Python installed
* On Ubuntu (18.04), the current approach is to use the built-in system Python3, as it is there by default
* The placement of plugin script files is flexible, allowing users to keep small scripts hanging, or develop entire
  Python packages of plugin scripts, with multiple classes per file, external library calls, etc.
* Currently, we are resolving the issues around adding extra libraries to the plugin system.  It is expected that adding
  pure Python libraries will be a very simple thing, and that using libraries with native code dependency will not be
  such a simple thing.
