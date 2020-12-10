#!/usr/bin/env bash

# Get the root directory of the repo

repo_root=$(git rev-parse --show-toplevel)

# Create a build directory in the root directory

mkdir -p $repo_root/build
cd $repo_root/build

# Use gcc 10

export C=/usr/bin/gcc-10
export CXX=/usr/bin/g++-10

# Run cmake and make

if [ -z ${NRP_INSTALL_DIR} ]; then 
    echo "NRP_INSTALL_DIR is unset"
    exit 1
fi

cmake .. -DCMAKE_INSTALL_PREFIX=$NRP_INSTALL_DIR
make -j
make install

# EOF
