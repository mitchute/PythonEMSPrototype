
Welcome to the EnergyPlus Plugin documentation!
===============================================

.. toctree::
   :maxdepth: 2
   :caption: Contents:

   background
   workflows
   python
   python_paths

Highest-level overview:

- `EnergyPlus <https://github.com/NREL/EnergyPlus>`_ is a whole building energy simulation tool.
- EnergyPlus had a system called EMS that allowed users to write custom code to modify the dynamics of the simulation.
- This has been expanded (and renamed) to a plugin system that uses Python scripts, called the EnergyPlus Plugin System, or EPS.
- This plugin system is used by creating Python scripts that inherit from the script base class and inserting calls into an EnergyPlus input file.
- A repository has been set up that allows users to develop against the predefined EPS interface `here <https://github.com/Myoldmopar/EnergyPlusPluginInterface>`_.
- A `Fake EnergyPlus <https://github.com/Myoldmopar/PythonEMSPrototype>`_ C++ project has been set up to prototype out the implementation.

Eventually, the prototype tool will be deleted, and the EnergyPlus implementation will fully document the EnergyPlus workflows.
The EPS interface repo will live on, but transferred to a different organization.

Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
