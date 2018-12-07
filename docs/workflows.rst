Python EMS Workflows
====================

This document discusses how a user will interact with the EnergyPlus Python EMS framework.
Currently, the tool is at a prototype stage, where a standalone C++ program mimics the behavior of EnergyPlus.
The exact details will change once the prototype is actually made into EnergyPlus (input definitions, etc.).
But the overall process is expected to be similar to that which is described here.

What even is EMS?
-----------------

EnergyPlus input files are written in the EnergyPlus Input Data File IDF syntax (or the recently added EpJSON format).
The inputs written this file include specifications of building geometry, surfaces, constructions, mechanical
equipment, schedules, and other boundary conditions that the governing equations inside EnergyPlus are solving.

When a user wants to add custom functionality such as a new control strategy, or custom scheduling, or capturing the
physical response of a custom piece of equipment, they can do this using the EnergyPlus Energy Management System (EMS).
That has involved adding inputs for sensors, actuators, and EMS programs, right alongside the rest of the EnergyPlus inputs.
The sensors are used as inputs to the program, and the actuators act as the effect, or the output of the EMS program.
The EMS programs are then called at specific points in a simulation and alter the output by affecting the actuator values.

EMS has enabled users to evaluate control strategies and pieces of equipment without the need for recompiling EnergyPlus itself.
It has also been used as a prototyping engine before new capabilities get implemented inside EnergyPlus natively.
As EMS has progressed further, more advanced usage has revealed that there is much room for improvement in terms of the EMS development environment.
Users struggle with organizing and managing large EMS programs, and have a very difficult time debugging EMS programs when running them.

EnergyPlus' new Python EMS framework will hopefully alleviate some of this difficulty, and open up a door to massive new
possibilities of doing prototyping and other custom applications with EnergyPlus.

How do I use PyMS?
------------------

Consider a user who is already utilizing the core EMS functionality of EnergyPlus, but wants to try out the Python implementation.
There are a few steps to getting it all working:

- Updated version of EnergyPlus
- Understanding what to write
- Writing the script contents
- Adding proper inputs to EnergyPlus
- Running EnergyPlus

Program Versions
++++++++++++++++

Right now, there are _zero_ versions of EnergyPlus with support for PyMS.
That will change soon, but for now, the PythonEMSPrototype tool mimics what EnergyPlus will do.

What do I write?
++++++++++++++++

So what does the script actually look like?
It's just a Python class, that should inherit from the EMSInterface base class, which is defined in the `PyMS repository <https://github.com/Myoldmopar/PyMS>`_.

   NOTE: The user does _not_ need to download that repository for EnergyPlus to work, as EnergyPlus will already have that version available.

   NOTE: What if I get the wrong version?  EnergyPlus will be able to report the version of PyMS it is using.
   In addition, PyMS will be semantically versioned, so E+ will have backward compatibility will older PyMS scripts with the same major version number.

If one looks at the EMSInterface class in that repo, with each function documented, it should become pretty clear which functions need to be overridden in a derived class.
For more advanced usage, that package could be downloaded into an existing Python installation (soon to be via PyPi), which would enable
users to develop their scripts with the intelligence of a Python IDE helping ensure they have their syntax and overridden functions set up properly.

   NOTE: The user does not need to install any version of Python to get PyMS working, as EnergyPlus packages its own Python library.
   The user must write Python code that is Python 3.6 compliant as that is the target version for PyMS currently.

In addition to just inspecting the base class, there will also be an example script made available.
It is currently in the prototype repo in the example directory.

What can the script do?
+++++++++++++++++++++++

To write the actual contents of the script, you must override just a few methods:

- get_calling_point

    - This should return an integer from the CallingPointsMirror enumeration, also defined in the py_ems_interface file

- get_sensed_data_list

    - This should return a list of string identifiers that match up with sensors available inside EnergyPlus
    - For right now it's just a magic set of hardwired available strings

- get_actuator_list

    - This should return a list of string identifiers that match up with actuators available inside EnergyPlus
    - For right now it's just a magic set of hardwired available strings

- ems_main

    - This is what is called at the defined calling point in EnergyPlus
    - This function is expected to do the heavy lifting of the EMS process, although it can call out to any other functions or classes as needed
    - The function should return floating point values in the same order as the actuator list from get_actuator_list

TODO: Add example here (they don't necessarily know sensor/actuator IDs until next section)
TODO: Also add description of running tester script

EnergyPlus Input File
+++++++++++++++++++++

Now that you have a PyMS script handy, you need to tell EnergyPlus to find it and use it.
This part will be a bit different once inside EnergyPlus, but the overall process should be similar:

- Set up sensors in EnergyPlus to make values available to PyMS
- Set up actuators in EnergyPlus to make them accessible to PyMS
- Match up those sensors and actuators with the values returned from the functions in the PyMS script
- You'll need to tell EnergyPlus where to find your PyMS directory

    - There is a lot of detail about Python paths in the (python_paths) document (put link there).
    - For now, suffice it to say that you need to keep a folder of your Python classes handy, and tell EnergyPlus where they live

- Then you'll need to tell EnergyPlus where to find your specific module, and the name of the class inside it.
  This will need to be available at one of the search paths you gave it

Run EnergyPlus
++++++++++++++

Then just run EnergyPlus through normal means.  EP-Launch, command line interface, whatever.
No need to start up Python or anything, EnergyPlus will automatically instantiate the Python library and start running, making calls to Python as needed.
The values returned from the PyMS main functions will be used to adjust the actuated values inside EnergyPlus, and affect the simulation results.
