#! /usr/bin/python
# Make a number of files for testing application definition
# finding, recursion etc.

import h5py
import numpy

# A multi entry files
def makeEntry(name,f):
	entry = f.create_group(name)
	entry.attrs['NX_class'] = numpy.string_('NXentry')
	dset = entry.create_dataset('definition',(1,),dtype="S70")
	dset[0] = "NXminimal"
	dset = entry.create_dataset('title',(1,),dtype="S70")
	dset[0] = "Gewindestange"


f = h5py.File('manyentries.h5','w')
f.attrs['file_name'] = numpy.string_('fileattributes.h5')
f.attrs['file_time'] = numpy.string_('2012-08-15T09:55:43+01:00')

makeEntry("entry0", f)
makeEntry("entry1", f)
makeEntry("entry2", f)

f.close()

# A file with a shitty application definition
f = h5py.File('shittyapp.h5','w')
f.attrs['file_name'] = numpy.string_('shittyapp.h5')
f.attrs['file_time'] = numpy.string_('2012-08-15T09:55:43+01:00')

entry = f.create_group("entry")
entry.attrs['NX_class'] = numpy.string_('NXentry')
dset = entry.create_dataset('definition',(1,),dtype="S70")
dset[0] = "NXshitty"

f.close

# A file using inheritance in the application definition
f = h5py.File('inherit.h5','w')
f.attrs['file_name'] = numpy.string_('inherit.h5')
f.attrs['file_time'] = numpy.string_('2012-08-15T09:55:43+01:00')

entry = f.create_group("entry")
entry.attrs['NX_class'] = numpy.string_('NXentry')
dset = entry.create_dataset('definition',(1,),dtype="S70")
dset[0] = "NXsuper"

dset = entry.create_dataset('title',(1,),dtype="S70")
dset[0] = "Gewindestange"

dset = entry.create_dataset('instrument',(1,),dtype="S70")
dset[0] = "Hammer"

f.close()

# A file with multiple subentries
def makeSubEntry(name,f):
	entry = f.create_group(name)
	entry.attrs['NX_class'] = numpy.string_('NXsubentry')
	dset = entry.create_dataset('definition',(1,),dtype="S70")
	dset[0] = "NXminimal"
	dset = entry.create_dataset('title',(1,),dtype="S70")
	dset[0] = "Gewindestange"

f = h5py.File('subentry.h5','w')
f.attrs['file_name'] = numpy.string_('subentry.h5')
f.attrs['file_time'] = numpy.string_('2012-08-15T09:55:43+01:00')

entry = f.create_group("entry")
entry.attrs['NX_class'] = numpy.string_('NXentry')

makeSubEntry("entry/sub1",f)
makeSubEntry("entry/sub2",f)
makeSubEntry("entry/sub3",f)

f.close()

# A file with a subentry missing a definition
f = h5py.File('subNOdef.h5','w')
f.attrs['file_name'] = numpy.string_('subNOdef.h5')
f.attrs['file_time'] = numpy.string_('2012-08-15T09:55:43+01:00')

entry = f.create_group("entry")
entry.attrs['NX_class'] = numpy.string_('NXentry')

entry = f.create_group("entry/sub1")
entry.attrs['NX_class'] = numpy.string_('NXsubentry')

f.close()

# A file for testing recursion
f = h5py.File('recurse.h5','w')
f.attrs['file_name'] = numpy.string_('recurse.h5')
f.attrs['file_time'] = numpy.string_('2012-08-15T09:55:43+01:00')

entry = f.create_group("entry")
entry.attrs['NX_class'] = numpy.string_('NXentry')
dset = entry.create_dataset('definition',(1,),dtype="S70")
dset[0] = "NXrecurse"
dset = entry.create_dataset('title',(1,),dtype="S70")
dset[0] = "Level0"

entry = f.create_group("entry/step1")
entry.attrs['NX_class'] = numpy.string_('NXentry')
dset = entry.create_dataset('title2',(1,),dtype="S70")
dset[0] = "Level1"

entry = f.create_group("entry/step1/step2")
entry.attrs['NX_class'] = numpy.string_('NXentry')
dset = entry.create_dataset('title3',(1,),dtype="S70")
dset[0] = "Level2"

f.close()
