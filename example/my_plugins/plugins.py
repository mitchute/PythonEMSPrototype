from energyplus_plugin.interface import EnergyPlusPlugin, CallingPointMirror
from special_py_package.special_functions import py_print


class MyPluginAfterSizing(EnergyPlusPlugin):
    """
    This class gives an example of using a plugin to update a coil size after auto-sizing is done.
    It's really just rounding the value to the nearest integer, which is dumb, but OK.
    The program senses the auto-sized value, and actuates the coil size value.
    """

    def __init__(self):
        super().__init__()
        py_print("Constructed plugin derived class: " + type(self).__name__)

    def get_calling_point(self):
        return CallingPointMirror.AFTER_SIZING

    def get_sensed_data_list(self):
        return ["initialCoilSize"]

    def get_actuator_list(self):
        return ["updatedCoilSize"]

    @staticmethod
    def adjust_coil_size(original_size):
        # this could be a really deep function, but right now it just rounds
        return round(original_size)

    def main(self):
        py_print("Inside main function of " + type(self).__name__)
        current_coil_size = self.my_sensed_data['initialCoilSize']
        new_coil_size = self.adjust_coil_size(current_coil_size)
        py_print("Current coil size: " + str(current_coil_size))
        py_print("Updated (rounded) coil size: %s!" % new_coil_size)
        return [new_coil_size]


class MyPluginInsideTimeStep(EnergyPlusPlugin):
    """
    This class gives an example of using a plugin to calculate a damper position based on zone temperature.
    It's essentially mocking the ability to adjust HVAC output based on zone temperature.
    The program senses two zone temperatures, and actuates two zone damper positions.
    The damper position is calculated by a simple linear function of zone temperature.
    """

    def __init__(self):
        super().__init__()
        py_print("Constructed plugin derived class: " + type(self).__name__)

    def get_calling_point(self):
        return CallingPointMirror.HVAC_TIME_STEP_LOOP

    def get_sensed_data_list(self):
        return ["zoneOneTemperature", "zoneTwoTemperature"]

    def get_actuator_list(self):
        return ["zoneOneDamperPosition", "zoneTwoDamperPosition"]

    @staticmethod
    def get_damper_position(zone_temperature):
        # assuming minimum damper at 21degC of 0.1 for min air flow
        # and assuming a maximum damper at 27degC of 1.0
        # linear regression gives this formula:
        damper_position = 0.15 * zone_temperature - 3.05
        if damper_position <= 0.1:
            py_print("*Damper at minimum position for air flow")
            damper_position = 0.1
        elif damper_position >= 1.0:
            py_print("*Damper at maximum position for full load")
            damper_position = 1.0
        return damper_position

    def main(self):
        py_print("Inside main function of " + type(self).__name__)
        zone_temp1 = self.my_sensed_data["zoneOneTemperature"]
        zone_damper1 = self.get_damper_position(zone_temp1)
        py_print("Current zone 1 temp = " + str(zone_temp1) + " results in damper position: " + str(zone_damper1))
        zone_temp2 = self.my_sensed_data["zoneTwoTemperature"]
        zone_damper2 = self.get_damper_position(zone_temp2)
        py_print("Current zone 2 temp = " + str(zone_temp2) + " results in damper position: " + str(zone_damper2))
        return [zone_damper1, zone_damper2]

class MyPluginUserDefinedComponentModel(EnergyPlusPlugin):
    """
    This class gives an example of using a plugin to calculate a plantcomponent:userdefined object
    This will test the callingpoint UserDefinedComponentModel.
    The program has an initialize and simulate program and sets values to actuators.
    """

    def __init__(self):
        super().__init__()
        py_print("Constructed plugin derived class: " + type(self).__name__)

    def get_calling_point(self):
        return CallingPointMirror.USER_DEFINED_COMPONENT_MODEL

    def get_sensed_data_list(self):
        return ["PCUD_Tin", "Qdot"]

    def get_actuator_list(self):
        return ["PCUD_Tout", "PCUD_Mdot_Request"]

    def main(self):
        py_print("Inside main function of " + type(self).__name__)
        Tin = self.my_sensed_data["PCUD_Tin"]
        Qdot = self.my_sensed_data["Qdot"]
        Tout = Tin + (Qdot / (0.5 * 4181.0))
        py_print("Current inlet temp = " + str(Tin) + " and Qdot = " + str(Qdot) + " results in outlet temp: " + str(Tout))
        Mdot = 0.5
        py_print("Constant flowrate of " + str(Mdot))
        return [Tout, Mdot]
