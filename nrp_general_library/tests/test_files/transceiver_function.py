from NRPPythonModule import *
from NRPGeneralPythonTestModule import TestInputDevice, TestOutputDevice

@SingleTransceiverDevice(keyword='device', id=DeviceIdentifier('out', 'type', 'engine'))
@TransceiverFunction("engine")
def transceiver_function(device):
    test_val = device.test_value
    
    ret_dev = TestInputDevice()
    ret_dev.test_value = str(test_val)
    
    return [ret_dev]

