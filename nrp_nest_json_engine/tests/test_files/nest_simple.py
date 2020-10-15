"""Init File. Imports nest and sets up a poisson generator, neuron, and voltmeter"""

import nest
import nest.voltage_trace

nest.set_verbosity("M_WARNING")
nest.ResetKernel()

neuron = nest.Create("iaf_psc_alpha")
noise = nest.Create("poisson_generator", 2)
voltmeter = nest.Create("voltmeter")

nest.SetStatus(noise, {"rate": 80000.0, "rate": 15000.0})

nest.Connect(noise, neuron, syn_spec={'weight': [[1.2, -1.0]], 'delay': 1.0})
nest.Connect(voltmeter, neuron)
