from libNRPPythonModule import *
from testModule import TestInputDevice, TestOutputDevice

# Invalid syntax
fsdavhufdsihjk = jfdaonv

@SingleTransceiverDevice(keyword='device', id=DeviceIdentifier('dev', 'type', 'engine'))
@TransceiverFunction()
def transceiver_function(device):
    test_val = device.test_value
    
    ret_dev = TestInputDevice()
    ret_dev.test_value = str(test_val)
    
    return [ret_dev]

