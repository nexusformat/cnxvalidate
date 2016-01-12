#! /usr/bin/python
# Make a file for testing the simple depends_on chain
# validation code

import h5py
import numpy

f = h5py.File('depends.h5','w')
f.attrs['file_name'] = numpy.string_('depends.h5')
f.attrs['file_time'] = numpy.string_('2012-08-15T09:55:43+01:00')

def makeSkeleton(name):
	entry = f.create_group(name)
	entry.attrs['NX_class'] = numpy.string_('NXentry')
	dset = entry.create_dataset('definition',(1,),dtype="S70")
	dset[0] = "NXdepends"
	sample = entry.create_group("sample")
	sample.attrs['NX_class'] = numpy.string_('NXsample')
	rot = sample.create_dataset('rotation_angle',(1,),'f32')
	rot[0] = 22.8
	trans = sample.create_group('transform')
	trans.attrs['NX_class'] = numpy.string_('NXtransformations')
	dset = trans.create_dataset('depends_on',(1,),dtype="S70")

def setCIF(ds, offset, vector, ttype, depends_on):
	ds.attrs['offset'] = offset
	ds.attrs['vector'] = vector
	ds.attrs['transformation_type'] = numpy.string_(ttype)
	ds.attrs['depends_on'] = numpy.string_(depends_on)


# First a case where everything is OK and using all the
# different ways to get at a field.
makeSkeleton('good')

ds = f['/good/sample/rotation_angle']
setCIF(ds,(0.,0.,0.),(0.,1.,0.),'rotation','transform/z')
trans = f['/good/sample/transform']

ds = trans.create_dataset('x',(1,),'f32')
ds[0] = 122.
setCIF(ds,(0.,0.,0.),(1.,0.,0.),'translation','/good/sample/rotation_angle')

ds = trans.create_dataset('z',(1,),'f32')
ds[0] = -23.
setCIF(ds,(0.,0.,0.),(0.,0.,1.),'translation','.')

transdep = f['/good/sample/transform/depends_on']
transdep[0] = numpy.string_('x')

# A case where the start point of the depends_on chain is wrong
makeSkeleton('startwrong')

ds = f['/startwrong/sample/rotation_angle']
setCIF(ds,(0.,0.,0.),(0.,1.,0.),'rotation','transform/z')
trans = f['/startwrong/sample/transform']

ds = trans.create_dataset('x',(1,),'f32')
ds[0] = 122.
setCIF(ds,(0.,0.,0.),(1.,0.,0.),'translation','/good/sample/rotation_angle')

ds = trans.create_dataset('z',(1,),'f32')
ds[0] = -23.
setCIF(ds,(0.,0.,0.),(0.,0.,1.),'translation','.')

transdep = f['/startwrong/sample/transform/depends_on']
transdep[0] = numpy.string_('schnueffel')


# An unterminated depends_on chain
makeSkeleton('unterminated')

ds = f['/unterminated/sample/rotation_angle']
setCIF(ds,(0.,0.,0.),(0.,1.,0.),'rotation','transform/z')
trans = f['/unterminated/sample/transform']

ds = trans.create_dataset('x',(1,),'f32')
ds[0] = 122.
setCIF(ds,(0.,0.,0.),(1.,0.,0.),'translation','/unterminated/sample/rotation_angle')

ds = trans.create_dataset('z',(1,),'f32')
ds[0] = -23.
setCIF(ds,(0.,0.,0.),(0.,0.,1.),'translation','.')
ds.attrs.__delitem__('depends_on')

transdep = f['/unterminated/sample/transform/depends_on']
transdep[0] = numpy.string_('x')

#A broken link in the depends_on chain
makeSkeleton('brokenlink')

ds = f['/brokenlink/sample/rotation_angle']
setCIF(ds,(0.,0.,0.),(0.,1.,0.),'rotation','transform/z')
trans = f['/brokenlink/sample/transform']

ds = trans.create_dataset('x',(1,),'f32')
ds[0] = 122.
setCIF(ds,(0.,0.,0.),(1.,0.,0.),'translation','rotation_angle')

ds = trans.create_dataset('z',(1,),'f32')
ds[0] = -23.
setCIF(ds,(0.,0.,0.),(0.,0.,1.),'translation','.')
ds.attrs.__delitem__('depends_on')

transdep = f['/brokenlink/sample/transform/depends_on']
transdep[0] = numpy.string_('x')

# A case with various attribute errors
makeSkeleton('badatt')

ds = f['/badatt/sample/rotation_angle']
setCIF(ds,(0.,0.,0.),(0.,1.,0.),'rotation','transform/z')
trans = f['/badatt/sample/transform']

ds = trans.create_dataset('x',(1,),'f32')
ds[0] = 122.
# Bad transformation_type, bad vector data type
setCIF(ds,(0.,0.,0.),(0,0,0),'truglation','/badatt/sample/rotation_angle')

ds = trans.create_dataset('z',(1,),'f32')
ds[0] = -23.
setCIF(ds,(0.,0.,0.),(0.,0.,1.),'translation','.')
# Bad offset type
ds.attrs['offset'] = numpy.string_('1.,0,0')
# Bad vector length
ds.attrs['vector'] = (1.0,0.,0.5,2.)

transdep = f['/badatt/sample/transform/depends_on']
transdep[0] = numpy.string_('x')


f.close()
