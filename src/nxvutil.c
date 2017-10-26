/**
 * This file contains some uitility functions for use within cnxvalidate. 
 * As of now, only the string attribute reading function
 *
 * Mark Koennecke, mark.koennecke@psi.ch, July 2016
 */
#include "nxvcontext.h"
#include <hdf5.h>
#include <hdf5_hl.h>
#include <string.h>

herr_t H5NXget_attribute_string( hid_t loc_id, const char *obj_name, const char *attr_name,  char *data )
{
    /* identifiers */
    hid_t      obj_id;
    hid_t attr_id;
    hid_t attr_type, space;
    char *varData;
    herr_t result;
    
    /* check the arguments */
    if (obj_name == NULL) 
      return -1;
    if (attr_name == NULL) 
      return -1;

    /* Open the object */
    if ((obj_id = H5Oopen( loc_id, obj_name, H5P_DEFAULT)) < 0) 
        return -1;

    if(( attr_id = H5Aopen(obj_id, attr_name, H5P_DEFAULT)) < 0) {
      H5Oclose(obj_id);
      return -1;
    }

    attr_type = H5Aget_type(attr_id);
    space = H5Aget_space(attr_id);

    if (H5Tis_variable_str(attr_type)) {
      hid_t btype = H5Tget_native_type(attr_type, H5T_DIR_ASCEND); 
      result = H5Aread(attr_id, btype, &varData);
      H5Tclose(btype);
      /*
	This is a memory issue waiting to happen. IMHO, there is a design issue with 
	H5LTget_attribute_string in itself that it does not pass in the length of the 
	string. Thus no check can be performed. 

        There is also a mmeory leak with varData. However, if I call the reclaim function 
        from the HDF5 API on it, I get a core dump.....
      */
      strcpy(data,varData);
    } else {
      result = H5Aread(attr_id,attr_type,data);
    }

    H5Tclose(attr_type);
    H5Sclose(space);
    H5Aclose(attr_id);
    H5Oclose(obj_id);

    return result;
}
/*-------------------------------------------------------------------------------
 A fixed version of H5LTread_dataset_string() which gest the fucking variable length 
 strings right.
 ----------------------------------------------------------------------------------*/

herr_t H5NXread_dataset_string( hid_t loc_id,
                               const char *dset_name,
                               char *buf )
{
    hid_t   did = -1;
    hid_t   tid = -1;
    H5T_class_t tclass;
    char **vstrdata;
    hid_t memtype_id;
    
    /* check the arguments */
    if (dset_name == NULL) 
      return -1;

    /* Open the dataset. */
    if((did = H5Dopen2(loc_id, dset_name, H5P_DEFAULT)) < 0)
        return -1;

    if((tid = H5Dget_type(did)) < 0)
        goto out;

    tclass = H5Tget_class(tid);
    if(H5Tis_variable_str(tid)) {
      /*
	This is a horrible cludge and may (will) cause memory issues. 
        Memory from reading the variable length string is not properly freed
        Copying the variable length data without length protection can cause 
        memory corruption if the space in buf is to small. But the length is not passed        in as a parameter... 
       */
      vstrdata = malloc(sizeof(char *));
      memtype_id = H5Tcopy(H5T_C_S1);
      H5Tset_size(memtype_id,H5T_VARIABLE);
      H5Dread(did,memtype_id,H5S_ALL,H5S_ALL,H5P_DEFAULT,vstrdata);
      buf[0] = '\0';
      strcpy(buf,vstrdata[0]);
      H5Tclose(memtype_id);
    } else {
      /* Read */
      if(H5Dread(did, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, buf) < 0)
        goto out;
    }

    return 0;

out:
    H5E_BEGIN_TRY {
        H5Dclose(did);


	H5Tclose(tid);
    } H5E_END_TRY;
    return -1;
}

