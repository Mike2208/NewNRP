"""Init File. Imports numpy"""

import nest
import numpy

numpy.random.rand()
numpy.random.RandomState(4)

pyrngs = [numpy.random.RandomState(s) for s in range(3, 5)]
