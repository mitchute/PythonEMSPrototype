# from ems_interface import EMSInterface0


my_variable = 1


def main(calling_point, data_array):
    global my_variable
    if calling_point == 'HVACTimeStepLoop':
        my_variable += data_array[0]
        print("   __Hit HVACTimeStepLoop, new value of my_variable = " + str(my_variable))
        return 0
    else:
        print("   __called from unused calling point \"%s\" __ignoring__" % calling_point)
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
