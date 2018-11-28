# from ems_interface import EMSInterface0


my_variable = 1


def main(data_value):
    global my_variable
    my_variable += data_value
    print("   __Hit HVACTimeStepLoop, new value of my_variable = " + str(my_variable))
    return 0

#
# class EMSExample1(EMSInterface0):
#
#     def __init__(self):
#         super().__init__()
#         self.my_variable = 1
#
#     def main(self, calling_point, data_array):
#         if calling_point == 'HVACTimeStepLoop':
#             self.my_variable += data_array[0]
#             print("Hit HVACTimeStepLoop, new value of my_variable = " + str(self.my_variable))
#         else:
#             print("__called from unused calling_point__ignoring__")
