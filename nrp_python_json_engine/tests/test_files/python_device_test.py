"""Simple python script file"""

from NRPPythonEngineModule import EngineScript,RegisterEngine

@RegisterEngine()
class Script(EngineScript):
    def initialize(self):
        self._registerDevice("device1")

    """Simple script function"""
    def runLoop(self, timestep):
        self._setDevice("device1", { "time" : self._time })
        dev = self._getDevice("device1")
        print("Engine 1 at time " + str(self._time))
        print("Device 1 data is " + str(dev))
