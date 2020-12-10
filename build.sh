#!/usr/bin/env bash

mkdir -p build
cd build
export C=/usr/bin/gcc-10
export CXX=/usr/bin/g++-10
cmake .. -DCMAKE_INSTALL_PREFIX=$NRP_INSTALL_DIR
make -j
make install