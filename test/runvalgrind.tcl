#!/usr/bin/tclsh

set flist [list badtimeattributes.h5 depends.h5 dssimpleatt.h5 dsunidim.h5 fileattributes.h5 \
	       group.h5 inherit.h5 link.h5 manyentries.h5 nofileattributes.h5 \
	       recurse.h5 shittyapp.h5 subentry.h5 subNOdef.h5]

foreach f $flist {
    catch {eval exec valgrind --leak-check=full ../nxvalidate -l data -d data/$f >& $f.val}
}

