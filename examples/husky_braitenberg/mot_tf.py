from NRPPythonModule import *
from NRPGazeboDevicesPython import PhysicsJoint
import NRPNestJSONPythonModule

@SingleTransceiverDevice(keyword='lwn', id=DeviceIdentifier('lwn', 'nest'))
@SingleTransceiverDevice(keyword='rwn', id=DeviceIdentifier('rwn', 'nest'))
@SingleTransceiverDevice(keyword='lpg', id=DeviceIdentifier('lpg', 'nest'))
@TransceiverFunction("gazebo")
def transceiver_function(lwn, rwn, lpg):
    back_left_j   = PhysicsJoint("husky::back_left_joint", "gazebo")
    back_right_j  = PhysicsJoint("husky::back_right_joint", "gazebo")
    front_left_j  = PhysicsJoint("husky::front_left_joint", "gazebo")
    front_right_j = PhysicsJoint("husky::front_right_joint", "gazebo")
    
    left_voltage = lwn.data['E_L']
    right_voltage = rwn.data['E_L']
    
    forward_vel = 1.0 * min(left_voltage, right_voltage)
    rot_vel = 1.0 * (right_voltage - left_voltage)

    print("lwn: " + str(lwn.data))
    print("rwn: " + str(rwn.data))
    print("lpg: " + str(lpg.data))

    print("Left voltage:  " + str(left_voltage))
    print("Right voltage: " + str(right_voltage))

    print("Forward velocity: " + str(forward_vel))
    print("Rotational vel:   " + str(rot_vel))

    back_left_j.velocity = forward_vel - rot_vel
    back_right_j.veclocity = -forward_vel - rot_vel

    front_left_j.velocity = forward_vel - rot_vel
    front_right_j.velocity = -forward_vel - rot_vel
   
    #return []
    return [ back_left_j, back_right_j, front_left_j, front_right_j ]

