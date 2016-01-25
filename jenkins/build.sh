#!/bin/bash

# Build script for execution on CI systems like jenkins.
# $1: Path to source code
# $2: Path to install into
# It puts the build directory in CWD.

cmake $1 -DCMAKE_BUILD_TYPE=Release 
         -DCMAKE_PREFIX_PATH=$2 -DCMAKE_INSTALL_PREFIX=$2

make -j 4
make install
