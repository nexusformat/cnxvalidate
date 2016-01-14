#! /usr/bin/python
# Make a file or several to test the group checking
# code of nxvalidate

import h5py
import numpy

f = h5py.File('group.h5','w')
f.attrs['file_name'] = numpy.string_('group.h5')
f.attrs['file_time'] = numpy.string_('2012-08-15T09:55:43+01:00')

entry = f.create_group('entry')
entry.attrs['NX_class'] = numpy.string_('NXentry')
dset = entry.create_dataset('definition',(1,),dtype="S70")
dset[0] = "NXgroup"

# A data item existing in the application definition
dset = entry.create_dataset('title',(1,),dtype="S70")
dset[0] = "Gewindestange"

# An additional data item existing in the base class
dset = entry.create_dataset('experiment_description',(1,),dtype="S70")
dset[0] = "Experiment in gewinde space"

# An additional data set not in the base class
dset = entry.create_dataset('experiment_ingredients',(1,),dtype="S70")
dset[0] = "Flour, eggs, baking powder"

# A group missing the NXclass attribute
noclass = entry.create_group("missing_class")

# A group in the application definition
data = entry.create_group('data')
data.attrs['NX_class'] = numpy.string_('NXdata')

# Test searching a group by typede
ds = data.create_group('data_sample')
ds.attrs['NX_class'] = numpy.string_('NXsample')


# An extra group not in the application definition
sample = entry.create_group('sample')
sample.attrs['NX_class'] = numpy.string_('NXsample')

# An extra group not in aplication definition or base class
# This provides a test case for a non existing base class too
data = entry.create_group('wool')
data.attrs['NX_class'] = numpy.string_('NXwool')

f.close()

f = h5py.File('misgroup.h5','w')
f.attrs['file_name'] = numpy.string_('misgroup.h5')
f.attrs['file_time'] = numpy.string_('2012-08-15T09:55:43+01:00')

entry = f.create_group('entry')
entry.attrs['NX_class'] = numpy.string_('NXentry')
dset = entry.create_dataset('definition',(1,),dtype="S70")
dset[0] = "NXgroup"


f.close()
