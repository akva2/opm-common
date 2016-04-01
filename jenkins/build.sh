#!/bin/bash

source `dirname $0`/build-opm-module.sh

# Clean workspace
git clean -xfd

# Build without MPI
pushd .
mkdir -p serial/build-opm-common
cd serial/build-opm-common
build_module "-DUSE_MPI=0" 1 $WORKSPACE
popd

cp serial/build-opm-common/testoutput.xml .
