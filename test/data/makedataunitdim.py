#! /usr/bin/python
# Make a file for testing dataset dimensions and units
# code of nxvalidate

import h5py
import numpy

f = h5py.File('dsunidim.h5','w')
f.attrs['file_name'] = numpy.string_('dsunidim.h5')
f.attrs['file_time'] = numpy.string_('2012-08-15T09:55:43+01:00')

entry = f.create_group('entry')
entry.attrs['NX_class'] = numpy.string_('NXentry')
dset = entry.create_dataset('definition',(1,),dtype="S70")
dset[0] = "NXunidim"

# The dimension stuff first
dset = entry.create_dataset('gooddata',(10,128,128), dtype='i32')
dset = entry.create_dataset('ranksymbol',(10,128,128), dtype='i32')
dset = entry.create_dataset('mismatchdim',(11,128,129), dtype='i32')
dset = entry.create_dataset('mismatchrank',(128,129), dtype='i32')
dset = entry.create_dataset('defsymbol',(10,), dtype='i32')
dset = entry.create_dataset('symbolok',(10,), dtype='i32')
dset = entry.create_dataset('symbolbad',(30,), dtype='i32')

# Units: integers
dset = entry.create_dataset('goodint',(1,), dtype='i32')
dset = entry.create_dataset('badint',(1,), dtype='f32')
dset = entry.create_dataset('goodposint',(1,), dtype='i32')
dset = entry.create_dataset('badposint',(1,), dtype='f32')
dset = entry.create_dataset('gooduint',(1,), dtype=numpy.dtype('uint32'))
dset = entry.create_dataset('baduint',(1,), dtype='i32')
dset = entry.create_dataset('worseuint',(1,), dtype='f32')


# Units: Floats
dset = entry.create_dataset('goodfloat',(1,), dtype='f32')
dset = entry.create_dataset('badfloat',(1,), dtype='i16')

# Units: NX_NUMBER
dset = entry.create_dataset('goodnumber',(1,), dtype='f32')
dset = entry.create_dataset('alsogoodnumber',(1,), dtype='i32')
dset = entry.create_dataset('badnumber',(1,), dtype='S70')


# NX_CHAR
dset = entry.create_dataset('goodtxt',(1,), dtype='S70')
dset = entry.create_dataset('badtxt',(1,), dtype='f32')

# Date and Time
dset = entry.create_dataset('goodtime',(1,), dtype='S70')
dset[0] = numpy.string_('2012-08-15T09:55:43+01:00')

dset = entry.create_dataset('moregoodtime',(1,), dtype='S70')
dset[0] = numpy.string_('2012-08-15 09:55:43+01:00')

dset = entry.create_dataset('badtime',(1,), dtype='i32')

dset = entry.create_dataset('badstringtime',(1,), dtype='S70')
dset[0] = numpy.string_('2012:08-15 09-55:43+01-00')

dset = entry.create_dataset('goodbin',(1,), dtype='int8')
dset = entry.create_dataset('badbin',(1,), dtype='i32')
dset = entry.create_dataset('reallybadbin',(1,), dtype='f32')

dset = entry.create_dataset('goodbool',(1,), dtype='int8')
dset = entry.create_dataset('badbool',(1,), dtype='i32')
dset = entry.create_dataset('reallybadbool',(1,), dtype='f32')

f.close()
