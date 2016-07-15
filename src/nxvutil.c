/**
 * This file contains some uitility functions for use within cnxvalidate. 
 * As of now, only the string attribute reaidng function
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
      */
      strcpy(data,varData);
      H5Dvlen_reclaim(attr_type, space, H5P_DEFAULT, &varData);
    } else {
      result = H5Aread(attr_id,attr_type,data);
    }

    H5Tclose(attr_type);
    H5Sclose(space);
    H5Aclose(attr_id);
    H5Oclose(obj_id);

    return result;
}
