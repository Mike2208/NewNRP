
# NRP Core - Backend infrastructure to synchronize simulations
#
# Copyright 2020 Michael Zechmair
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# This project has received funding from the European Union’s Horizon 2020
# Framework Programme for Research and Innovation under the Specific Grant
# Agreement No. 945539 (Human Brain Project SGA3).

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

