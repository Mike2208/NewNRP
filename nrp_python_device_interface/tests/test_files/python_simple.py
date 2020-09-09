"""Simple python script file"""

from NRPPythonEngineModule import EngineScript

@RegisterEngine()
class Script(EngineScript):
    """Simple script function"""
    def runLoop(self, timestep):
        print("Engine 1 at time " + str(self._time))
