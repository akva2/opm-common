#!/bin/bash

function build_config {
  cmake .. -DCMAKE_BUILD_TYPE=Release -DUSE_MPI=$1
  test $? -eq 0 || exit 1
  make test-suite
  test $? -eq 0 || exit 2
  ctest -T Test --no-compress-output
  convert.py -x conv.xsl -t . > testoutput.xml
}

# Clean workspace
git clean -xfd

# Build without MPI
pushd .
mkdir build-serial
cd build-serial
build_config(0)
popd

# Build with MPI
mkdir build-mpi
cd build-mpi
build_config(1)
popd

# Concat test results
xml_grep --wrap testsuites --cond testsuite build-serial/testoutput.xml build-mpi/testoutput.xml > testoutput.xml
