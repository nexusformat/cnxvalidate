
NeXus validation tool written in C
=====================================

# nxvalidate
This is the first version of nxvalidate written in C. Its dependencies
are libxml2 and the HDF5 libraries, version 1.8.9 or better. Its purpose is
to validate HDF5 files against NeXus application definitions. If you do not
know what all this is, see the [NeXus web site](http://www.nexusformat.org).

This repository holds two things:

* A library implementation which allows to use nxvalidate from your own
  software
* A proof of concept command line utility

# Using the nxvalidate Command Line Utility

nxvalidate is invoked like this:

    nxvalidate -a appdef -l appdefdir -p path  -o -b -u -d -t datafile


The minimum invocation is:

    nxvalidate -l appdefdir datafile

This searches for application definitions in appdefdir and validates datafile.
It expects to find the application definition to validate from from the
definitions fields in NXentry or NXsubentry.

All options:

* **-a appdef** Validate the datafile against the application definition appdef.
  This option allows to override the  application definition to validate against.
* **-l appdefdir** specifies the directory where to search for application
   definitions. As nxvalidate tries to load base calss definitions too, this must
	 be given.
* **-p path** specifies a path in the HDF5 file to validate. This also requires
  an application definition to be specified with -a.
* **-t** Produce all output possible
* **-d** Produce debug output tracing what nxvalidate does
* **-b** Warn about additional elements in the data file found in a base class
* **-u** Warn about undefined additional elements in the data file
* **-o** Warn about optional elements which are not present in the datafile
* **-e** Neaten the output with more whitespace

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
4. Edit CMakeLists.txt at the root level of the distribution. Due to a bug in
   cmake, you have to set HDF5_HL_LIBRARIES to point to the hdf5_hl library.
   The bug is already fixed in cmake but only very recently. Thus you may not
   have a fixed version of cmake.
5. cd into the build diretory with cd build
6. Run cmake: cmake ../
7. Run make: make
8. Run: make test

This should get it done.
When this job has been finished, there may be binary builds as well.
