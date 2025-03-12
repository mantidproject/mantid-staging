* In order to be able to use xvnc in a matrix job, the "Disable Xvnc execution on this node" should be selected for all Mac nodes.
* Machines that are set up to build Mantid should have the label linux-64, conda-build-osx or win-64.
* Machines that have any of the labels linux-64, conda-build-osx or win-64 will be considered eligible for running system tests. Be sure not to add these specific labels if a machine should not be used for system tests!
