#! /usr/bin/python
# Generates a rump HDF5 file without file attributes

import h5py
import numpy

# Missing file attributes
f = h5py.File('nofileattributes.h5','w')

entry = f.create_group('entry')
entry.attrs['NX_class'] = numpy.string_('NXentry')

f.close()

# Good file attributes
f = h5py.File('fileattributes.h5','w')
f.attrs['file_name'] = numpy.string_('fileattributes.h5')
f.attrs['file_time'] = numpy.string_('2012-08-15T09:55:43+01:00')

entry = f.create_group('entry')
entry.attrs['NX_class'] = numpy.string_('NXentry')

f.close()

# Malformed time attribute
f = h5py.File('badtimeattributes.h5','w')
f.attrs['file_name'] = numpy.string_('badtimeattributes.h5')
f.attrs['file_time'] = numpy.string_('2012:08-15T09:55:43+01:00')

entry = f.create_group('entry')
entry.attrs['NX_class'] = numpy.string_('NXentry')

f.close()
