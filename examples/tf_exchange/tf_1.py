from NRPPythonModule import *

@SingleTransceiverDevice(keyword='device1', id=DeviceIdentifier('device1', 'python_1', 'py_obj'))
@TransceiverFunction("python_2")
def transceiver_function(device1):
    rec_device1 = PythonDevice(device1.id, device1.data)

    rec_device1.name = "rec_device1"
    rec_device1.engine_name = "python_2"

    return [ rec_device1 ]

