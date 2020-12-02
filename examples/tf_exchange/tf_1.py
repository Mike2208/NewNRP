from NRPPythonModule import *

@SingleTransceiverDevice(keyword='device1', id=DeviceIdentifier('device1', 'python_1'))
@TransceiverFunction("python_2")
def transceiver_function(device1):
    rec_device1 = PythonDevice("rec_device1", "python_2")
    rec_device1.data = device1.data

    return [ rec_device1 ]

