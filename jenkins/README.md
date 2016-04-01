OPM-common jenkins build scripts:
--------------------------------

[B]build-opm-common.sh[/B]:
This expects to run on a jenkins with opm-common as the 'origin'.

It will build and execute tests with and without MPI.

[B]build-opm-common-pr.sh[/B]:
This expects to run on a jenkins with opm-common as the 'origin' remote
using the github PR builder plugin.

It will build and test opm-common with the specified pull request, with
and without MPI.
