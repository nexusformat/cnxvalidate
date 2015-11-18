
NeXus validation tool written in C
=====================================

# Alpha Warning
As of November 2015, this is an early alpha. Its mostly feature complete
but misses a proper test suite. Which is definitely needed for this piece
of software. You can attempt using it, but you have a good chance of it
crashing on you or giving false results. Please report any issues with
nxvalidate as github issues.

This is the first version of nxvalidate written in C. Its dependencies
are libxml2 and the HDF5 libraries. Its purpose it to validate a NeXus file
against a NeXus application definition. If you do not what all this is, see
the [NeXus WWW-site](http://www.nexusformat.org).

This repository holds two things:

* A library implementation which allows to use nxvalidate from your own
  software
* A proof of concept command line utility

# Using the nxvalidate Command Line Utility

nxvalidate is invoked like this:

    nxvvalidate -a appdef -l appdefdir -o -b -u -d -t datafile


The minimum invocation is:

    nxvvalidate -l appdefdir datafile

This searches for application definitions in appdefdir and validates datafile.
It expects to find the application definition to validate from from the
definitions fields in NXentry or NXsubentry.

All options:

* **-a appdef** Validate the datafile against the application definition appdef.
  This option allows to override the  application definition to validate against.
* **-l appdefdir** specifies the directory where to search for application
   definitions. As nxvalidate tries to load base calss definitions too, this must
	 be given.
* **-t** Produce all output possible
* **-d** Produce debug output tracing what nxvalidate does
* **-b** Warn about additional elements in the data file found in a base class
* **-u** Warn about undefined additional elements in the data file
* **-o** Warn about optional elements which are not present in the datafile

Validation errors are always reported.

# Using the nxvalidation library

1. You must include nxvalidate.h in your code
2. You must link your program against the nxvalidate, xml2, hdf5 and hdf_hl
   libraries.

The library documentation is basically the doxygen  markup of nxvalidate.h
and the example use case of the command line application in nxvmain.c.

# Building nxvalidate

Currently you have to build nxvalidate from source. 

1. Download or clone the repository
2. cd into it the source tree
3. Create a build directory: mkdir build
4. cd into the build diretory with cd build
5. Run cmake: cmake ../
6. Run make: make

This should get it done. Due to a bug in cmake, CMakeLists.txt contains an
ugly hack which  defines symbol for setting HDF5_HL_LIBRARIES. You may need
to edit this for your system.

When this job has been finished, there may be binary builds as well.
