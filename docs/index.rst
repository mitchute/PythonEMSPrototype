
Welcome to EnergyPlus Python Scripting's documentation!
===========================================

.. toctree::
   :maxdepth: 2
   :caption: Contents:

   background
   workflows
   python
   python_paths

Highest-level overview:

- `EnergyPlus <https://github.com/NREL/EnergyPlus>`_ is a whole building energy simulation tool.
- EnergyPlus Python Scripting (EPPS) is a Python interface to allow plugging in a Python script to the simulation.
- This interface is exercised by creating Python scripts that inherit from the script base class and inserting calls into an EnergyPlus input file.
- A repository has been set up that allows users to develop against the predefined PyMS interface `here <https://github.com/Myoldmopar/PyMS>`_.
- The `PythonEMSPrototype <https://github.com/Myoldmopar/PythonEMSPrototype>`_ is a C++ project that demonstrates how the implementation will look eventually in EnergyPlus.

Eventually, the prototype tool will be deleted, and the EnergyPlus implementation will fully document the EnergyPlus workflows.
The PyMS repo will live on, but probably transferred to a different organization.

Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
