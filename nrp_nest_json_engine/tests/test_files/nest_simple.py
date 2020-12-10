
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
