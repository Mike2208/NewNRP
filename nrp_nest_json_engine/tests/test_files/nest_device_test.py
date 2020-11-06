"""Init File. Imports nest and sets up a poisson generator, neuron, and voltmeter"""

import numpy
import nest
import nest.voltage_trace
from NRPNestPythonModule import CreateDevice

numpy.random.random()

nest.set_verbosity("M_WARNING")
nest.ResetKernel()

# Seed Python RNGs?
msd = 123456
N_vp = nest.GetKernelStatus(['total_num_virtual_procs'])[0]
pyrngs = [numpy.random.RandomState(s) for s in range(msd, msd+N_vp)]

# Seed global RNG
nest.SetKernelStatus({'grng_seed' : msd+N_vp})

# Seed per-process RNGs
nest.SetKernelStatus({'rng_seeds' : range(msd+N_vp+1, msd+2*N_vp+1)})

neuron = nest.Create("iaf_psc_alpha")
noise = nest.Create("poisson_generator", 2)
#voltmeter = nest.Create("voltmeter", "voltmeter")
voltmeter = CreateDevice("voltmeter", "voltmeter")

nest.SetStatus(noise, {"rate": 80000.0, "rate": 15000.0})

nest.Connect(noise, neuron, syn_spec={'weight': [[1.2, -1.0]], 'delay': 1.0})
nest.Connect(voltmeter, neuron)
