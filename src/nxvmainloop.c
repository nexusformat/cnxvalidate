/**
  * This file implement the main entry point for the validator.
	* It is responsible for locating validatable content and starting
	* the recursive validation of the data file.
	*
	* Copyright: GPL
	*
	* Mark Koennecke, mark.koennecke@psi.ch, and NIAC
	*/
#include <nxvalidate.h>
#include <hdf5.h>
#include <hdf5_hl.h>
#include <libxml/tree.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "nxvcontext.h"

int testISO8601(char *date)
{
	int y,m,d,h,min;
	int count;

	/*
		we accept the official format with the T between data and time
		and the unofficial common use where the T is replaced by a space
	*/
	count = sscanf(date,"%d-%d-%dT%d:%d", &y,&m,&d,&h,&min);
	if(count != 5){
		count = sscanf(date,"%d-%d-%d %d:%d", &y,&m,&d,&h,&min);
		if(count == 5){
			return 1;
		} else {
			return 0;
		}
	} else {
		return 1;
	}
}
/*----------------------------------------------------------------------*/
static int validateFileAttributes(pNXVcontext self, hid_t fileID)
{
	herr_t attID;
	char data[1024];

	NXVsetLog(self,"sev","debug");
	NXVsetLog(self,"message","Validating file attributes");
	NXVlog(self);

	memset(data,0,sizeof(data));

	NXVsetLog(self,"dataPath","/");
	attID = H5NXget_attribute_string(fileID,"/","file_name",data);
	if(attID < 0){
		NXVsetLog(self,"sev","error");
		NXVsetLog(self,"message","Missing required global file_name attribute");
		NXVlog(self);
		self->errCount++;
	}

	attID = H5NXget_attribute_string(fileID,"/","file_time",data);
	if(attID < 0){
		NXVsetLog(self,"sev","error");
		NXVsetLog(self,"message","Missing required global file_time attribute");
		NXVlog(self);
		self->errCount++;
	} else {
			if(!testISO8601(data)){
				NXVsetLog(self,"sev","error");
				NXVsetLog(self,"message","file_time is not in ISO 8601 format");
				NXVlog(self);
				self->errCount++;
			}
	}

	return 0;
}
/*--------------------------------------------------------------*/
static int validatePath(pNXVcontext self, char *path, char *rawNxdlFile)
{
	int status;
	hid_t groupID;
	xmlNodePtr groupNode = NULL;
	char nxdlFile[512];

	/*
		first fix the appDef: the nxdl.xml postfix may be missing
	*/
	if(strstr(rawNxdlFile,"nxdl.xml") != NULL){
		strncpy(nxdlFile,rawNxdlFile,sizeof(nxdlFile));
	} else {
		snprintf(nxdlFile,sizeof(nxdlFile),"%s.nxdl.xml", rawNxdlFile);
	}

	/*
		Save the entry we are validating for depends_on
	*/
	if(self->validationRoot != NULL){
		self->validationRoot = strdup(path);
	}

	/* report what is being done */
	NXVsetLog(self,"dataPath", path);
	NXVsetLog(self,"sev","debug");
	NXVsetLog(self,"definition", nxdlFile);
	NXVprintLog(self,"message","Validating %s against %s",
		path,nxdlFile);
	NXVlog(self);

	/* load the application definition */
	status = NXVloadAppDef(self,nxdlFile);
	if(status != 0){
		return status;
	}

	groupID = H5Gopen(self->fileID, path,H5P_DEFAULT);
	assert(groupID >= 0); /* we searched for it. This cannot be */
	groupNode = NXVfindEntry(self);
	if(groupNode == NULL){
		return 1;
	}

	self->nxdlPath = NULL;
	status = NXVvalidateGroup(self, groupID, groupNode);
	xmlFreeDoc(self->nxdlDoc);
	self->nxdlDoc = NULL;
	return status;
}
/*---------------------------------------------------------------*/
static herr_t NXVentryIterator(hid_t g_id,
		const char *name,
		const H5L_info_t *info, void *op_data)
{
	pNXVcontext self = (pNXVcontext)op_data;
  H5O_info_t obj_info;
	hid_t attrID, defID, subID;
  char nxClass[256], definition[1024], nxPath[512];

	memset(nxClass,0,sizeof(nxClass));
	memset(definition,0,sizeof(definition));

	H5Oget_info_by_name(g_id, name, &obj_info,H5P_DEFAULT);
	if(obj_info.type == H5O_TYPE_GROUP){
		attrID = H5NXget_attribute_string(g_id,name,
			"NX_class", nxClass);
		if(attrID >= 0 && strcmp(nxClass,"NXsubentry") == 0)	{
			subID = H5Gopen(g_id,name,H5P_DEFAULT);
			defID = H5LTread_dataset_string (subID,
				 "definition", definition );
			H5Iget_name(subID,nxPath,sizeof(nxPath));
			if(defID >= 0){
				validatePath(self,nxPath,definition);
				self->subEntryFound = 1;
			} else {
				NXVsetLog(self,"sev","error");
				NXVsetLog(self,"dataPath",nxPath);
				NXVprintLog(self,"message",
				"Cannot validate %s, no application definition",nxClass);
				NXVlog(self);
				H5Iget_name(g_id,nxPath,sizeof(nxPath));
				NXVsetLog(self,"dataPath",nxPath);
			}
			H5Gclose(subID);
		}
	}
	return 0;
}
/*---------------------------------------------------------------*/
static herr_t NXVrootIterator(hid_t g_id,
		const char *name,
		const H5L_info_t *info, void *op_data)
{
	pNXVcontext self = (pNXVcontext)op_data;
  H5O_info_t obj_info;
  hid_t attrID, entryID, defID;
	hsize_t idx = 0;
	char nxClass[132], nxPath[256], definition[1024];

	memset(nxClass,0,sizeof(nxClass));
	memset(definition,0,sizeof(definition));

	H5Oget_info_by_name(g_id, name, &obj_info,H5P_DEFAULT);
	if(obj_info.type == H5O_TYPE_GROUP){
		  /*
			  work the NX_class attribute
		  */
			attrID = H5NXget_attribute_string(g_id,name,
				"NX_class", nxClass);
			if(attrID < 0){
					NXVsetLog(self,"sev","info");
					NXVprintLog(self,"message",
					"No NX_class attribute on root group /%s", name);
					NXVlog(self);
			} else {
					if(strcmp(nxClass,"NXentry") != 0){
						NXVsetLog(self,"sev","error");
						NXVprintLog(self,"dataPath","/%s",name);
            NXVprintLog(self,"message",
							"Wrong root group class %s, expected NXentry",nxClass);
						NXVlog(self);
					}
			}

			/*
				if the user specified an application definition,
				validate against that one
			*/
			snprintf(nxPath, sizeof(nxPath),"/%s", name);
			if(self->nxdlFile != NULL){
					validatePath(self,nxPath,self->nxdlFile);
					return 0;
			}

			/*
				we have no application definition: look for one!
				Do we have a definitions field?
			*/
			entryID = H5Gopen(g_id,name,H5P_DEFAULT);
			if(entryID < 0){
				NXVsetLog(self,"sev","error");
				NXVprintLog(self,"dataPath","/%s",name);
				NXVprintLog(self,"message",
					"Unexpectedly failed to open group %s",name);
				NXVlog(self);
				self->errCount++;
				return 0;
			}
			defID = H5LTread_dataset_string (entryID,
				 "definition", definition );
			if(defID >= 0){
				validatePath(self,nxPath,definition);
			} else {
				/*
					we are either missing something to validate against
					or we have NXsubentry
				*/
				self->subEntryFound = 0;
				H5Literate(entryID, H5_INDEX_NAME, H5_ITER_INC, &idx,
					NXVentryIterator, self);
				if(self->subEntryFound == 0){
					NXVsetLog(self,"sev","error");
					NXVprintLog(self,"dataPath","/%s",name);
					NXVprintLog(self,"message",
						"Cannot validate %s, failed to find application definition",
						name);
					self->errCount++;
					NXVlog(self);
				}
			}

			H5Gclose(entryID);
	} /* end of group type IF */
  /*
	  ignore datasets and other stuff at root level
	*/
	return 0;
}
/*---------------------------------------------------------------*/
int NXVvalidate(pNXVcontext self, const char *dataFile,
	        const char *nxdlFile, const char *path)
{
	hid_t fileID, gid;
	hsize_t idx = 0;
  int status = 0, returnVal = 0;

  NXVprepareContext(self,(char *)dataFile,(char *)nxdlFile);


	/*
		switch off HDF5 error messages
	*/
  H5Eset_auto2( H5E_DEFAULT, NULL, NULL );

  fileID = H5Fopen(self->dataFile,H5F_ACC_RDONLY ,H5P_DEFAULT );
	if(fileID< 0){
		NXVsetLog(self, "sev","fatal");
		NXVsetLog(self, "message","Failed to open data file");
		NXVlog(self);
		return 1;
	}
	self->fileID = fileID;

	status = validateFileAttributes(self, fileID);
	if(status != 0){
		returnVal = 1;
	}

	/*
	* we have been give an explicit path to validate
	*/
  if(path != NULL){
		if(nxdlFile == NULL){
			NXVsetLog(self,"sev","fatal");
			NXVsetLog(self,"message",
			"Need application definition parameter when validating a specific path");
			NXVlog(self);
			self->errCount++;
			return 1;
		}
    status = validatePath(self,(char *)path,(char *)nxdlFile);
	} else {
		/*
		  we iterate over all root level groups in
			the file
		*/
		gid = H5Gopen(fileID,"/",H5P_DEFAULT);
		if(gid < 0){
			NXVsetLog(self, "sev","fatal");
			NXVsetLog(self, "message","Failed to open root group");
			NXVlog(self);
			return 1;
		}
		H5Literate(gid, H5_INDEX_NAME, H5_ITER_INC, &idx,
			NXVrootIterator, self);
		H5Gclose(gid);
	}

	H5Fclose(fileID);

	if(self->errCount > 0){
		returnVal = 1;
	}
	return returnVal;
}
