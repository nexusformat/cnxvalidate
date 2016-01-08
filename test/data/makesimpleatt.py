#! /usr/bin/python
# Make a file for testing the simple dataset attribute
# validation code of nxvalidate

import h5py
import numpy

f = h5py.File('dssimpleatt.h5','w')
f.attrs['file_name'] = numpy.string_('dssimpleatt.h5')
f.attrs['file_time'] = numpy.string_('2012-08-15T09:55:43+01:00')

entry = f.create_group('entry')
entry.attrs['NX_class'] = numpy.string_('NXentry')
dset = entry.create_dataset('definition',(1,),dtype="S70")
dset[0] = "NXsimpleatt"

# For testing the signal attribute
dset = entry.create_dataset('intsignal',(128,128), dtype='i32')
dset.attrs['signal'] = 1
dset = entry.create_dataset('txtsignal',(128,128), dtype='i32')
dset.attrs['signal'] = numpy.string_('1')
dset = entry.create_dataset('badsignal',(128,128), dtype='i32')
dset.attrs['signal'] = 1.8
dset = entry.create_dataset('wrongsignal',(128,128), dtype='i32')
dset.attrs['signal'] = 2


# For testing the primary attribute
dset = entry.create_dataset('intprim',(128,128), dtype='i32')
dset.attrs['primary'] = 1
dset = entry.create_dataset('txtprim',(128,128), dtype='i32')
dset.attrs['primary'] = numpy.string_('1')
dset = entry.create_dataset('badprim',(128,128), dtype='i32')
dset.attrs['primary'] = 1.8
dset = entry.create_dataset('wrongprim',(128,128), dtype='i32')
dset.attrs['primary'] = 2

# For testing the axis attribute
dset = entry.create_dataset('intaxis',(128,), dtype='f32')
dset.attrs['axis'] = 1
dset = entry.create_dataset('txtaxis',(128,), dtype='f32')
dset.attrs['axis'] = numpy.string_('1')
dset = entry.create_dataset('badaxis',(128,), dtype='f32')
dset.attrs['axis'] = 1.8
dset = entry.create_dataset('wrongaxis',(128,), dtype='f32')
dset.attrs['axis'] = 2

# For testing the axes attribute
dset = entry.create_dataset('commaaxes',(128,128), dtype='i32')
dset.attrs['axes'] = numpy.string_("intaxis,txtaxis")
dset = entry.create_dataset('colonaxes',(128,128), dtype='i32')
dset.attrs['axes'] = numpy.string_("intaxis:txtaxis")
dset = entry.create_dataset('brokenaxes',(128,128), dtype='i32')
dset.attrs['axes'] = 5
dset = entry.create_dataset('morebrokenaxes',(128,128), dtype='i32')
dset.attrs['axes'] = numpy.string_("intaxis,txtaxis")
dset = entry.create_dataset('invalidaxes',(128,128), dtype='i32')
dset.attrs['axes'] = numpy.string_("gargel,gurks")

# For testing offset and stride
dset = entry.create_dataset('goodoffsetstride',(128,128), dtype='i32')
dset.attrs['data_offset'] = [0,0]
dset.attrs['stride'] = [1,1]
dset = entry.create_dataset('baddimoffsetstride',(128,128), dtype='i32')
dset.attrs['data_offset'] = [0,]
dset.attrs['stride'] = [1,]
dset = entry.create_dataset('badtypeoffsetstride',(128,128), dtype='i32')
dset.attrs['data_offset'] = [0.,0.]
dset.attrs['stride'] = [1.,1.]
dset = entry.create_dataset('onemissingoffsetstride',(128,128), dtype='i32')
dset.attrs['data_offset'] = [0,0]

# For testing the calibration attribute
dset = entry.create_dataset('calnominal',(128,128), dtype='i32')
dset.attrs['calibration_status'] = numpy.string_("Nominal")
dset = entry.create_dataset('calnmes',(128,128), dtype='i32')
dset.attrs['calibration_status'] = numpy.string_("Measured")
dset = entry.create_dataset('calwrong',(128,128), dtype='i32')
dset.attrs['calibration_status'] = numpy.string_("Noergel")
dset = entry.create_dataset('calbadtype',(128,128), dtype='i32')
dset.attrs['calibration_status'] = 3.14

#For testing the interpretation attribute
dset = entry.create_dataset('intscaler',(128,128), dtype='i32')
dset.attrs['interpretation'] = numpy.string_("scaler")
dset = entry.create_dataset('intspec',(128,128), dtype='i32')
dset.attrs['interpretation'] = numpy.string_("spectrum")
dset = entry.create_dataset('intimage',(128,128), dtype='i32')
dset.attrs['interpretation'] = numpy.string_("image")
dset = entry.create_dataset('intrgba',(128,128), dtype='i32')
dset.attrs['interpretation'] = numpy.string_("rgba-image")
dset = entry.create_dataset('inthsla',(128,128), dtype='i32')
dset.attrs['interpretation'] = numpy.string_("hsla-image")
dset = entry.create_dataset('intcmyk',(128,128), dtype='i32')
dset.attrs['interpretation'] = numpy.string_("cmyk-image")
dset = entry.create_dataset('intvertex',(128,128), dtype='i32')
dset.attrs['interpretation'] = numpy.string_("vertex")
dset = entry.create_dataset('intbad',(128,128), dtype='i32')
dset.attrs['interpretation'] = numpy.string_("cucumber")
dset = entry.create_dataset('intbadtype',(128,128), dtype='i32')
dset.attrs['interpretation'] = 1




f.close()
