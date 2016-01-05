#! /usr/bin/python
# Make a file or several to test the link checking
# code of nxvalidate

import h5py
import numpy

f = h5py.File('link.h5','w')
f.attrs['file_name'] = numpy.string_('link.h5')
f.attrs['file_time'] = numpy.string_('2012-08-15T09:55:43+01:00')

entry = f.create_group('entry')
entry.attrs['NX_class'] = numpy.string_('NXentry')
dset = entry.create_dataset('definition',(1,),dtype="S70")
dset[0] = "NXlink"

target = f.create_group('target')
target.attrs['NX_class'] = numpy.string_('NXentry')
targettitle = target.create_dataset('title',(1,),dtype="S70")
targettitle[0] = "Fische sind Nass"
targettitle.attrs['target'] = numpy.string_('/target/title')

# Another field but without a target attribute
targetrev = target.create_dataset('revision',(1,),dtype="S70")
targetrev[0] = "Version 7.8"

# Another field with a wrong target attribute
targetdes = target.create_dataset('description',(1,),dtype="S70")
targetdes[0] = "Bloody experiment"
targetdes.attrs['target'] = numpy.string_('/target/guggelhupf')

# Now create the links
entry['title'] = targettitle
entry['revision'] = targetrev
#entry._id.link('/target/revision','revision',h5py.h5g.LINK_HARD)
entry['description'] = targetdes
entry['hugo'] = h5py.SoftLink('/target/doesnotexist')

f.close()
