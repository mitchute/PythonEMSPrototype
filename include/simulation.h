#ifndef ENERGYPLUSPYEMS_SIMULATION_H
#define ENERGYPLUSPYEMS_SIMULATION_H

// I initially thought we'd have a separate structure for passing "broad simulation state" data to Python
// but I kinda think now we should just be more explicit about it and make the users define them as sensors.
//struct SimulationStructure {
//	double outdoorDryBulb = 0;
//	double windSpeed = 0;
//};

struct SensedVariables {
	double zoneOneTemperature = 0;
	double zoneTwoTemperature = 0;
	double initialCoilSize = 0;
  double PCUD_Tin = 0;
  double Qdot = 0;
};

struct ActuatedVariables {
	double zoneOneDamperPosition = 0;
	double zoneTwoDamperPosition = 0;
	double updatedCoilSize = 0;
  double PCUD_Tout = 0;
  double PCUD_Mdot_Request = 0;
};

#endif //ENERGYPLUSPYEMS_SIMULATION_H
