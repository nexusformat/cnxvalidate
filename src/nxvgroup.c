/**
  * This file contains all the code for validating groups
	*
	* copyright: GPL
	*
	* Mark Koennecke, mark.koennecke@psi.ch, and NIAC
	*/
#include <assert.h>
#include <nxvalidate.h>
#include "nxvcontext.h"
#include <string.h>
#include <hdf5.h>
#include <hdf5_hl.h>
#include <libxml/tree.h>

/*--------------------------------------------------------------*/
static int isOptional(xmlNodePtr node)
{
	xmlChar *min= NULL;
	xmlChar *opt= NULL;
	xmlChar *name= NULL;
	xmlChar *rec= NULL;

	int num;
	int istrue;

	min = xmlGetProp(node,(xmlChar *)"minOccurs");
	opt = xmlGetProp(node,(xmlChar *)"optional");
	rec = xmlGetProp(node,(xmlChar *)"recommended");
	name = xmlGetProp(node,(xmlChar *)"name");       

	if(min == NULL && opt == NULL && rec == NULL){
		return 0;
	}
	if(min != NULL) {
		num = atoi((char *)min);
		if(num == 0){
			return 1;
		} else {
			return 0;
		}
	}
	if(opt != NULL) {
		istrue = 1;
		if (strcmp((char *)opt,"false")==0) istrue=0;
		return istrue;
	}
	if(rec != NULL) {
		istrue = 1;
		if (strcmp((char *)rec,"false")==0) istrue=0;
		return istrue;
	}
	return 0;
}

static void validateGroupAttributes(pNXVcontext self,
	hid_t groupID, xmlNodePtr groupNode)
{
	xmlNodePtr cur;
	xmlChar *type, *name;
	herr_t attID;
	char data[512], gname[512];

	memset(data,0,sizeof(data));
	memset(gname,0,sizeof(gname));
	H5Iget_name(groupID,gname,sizeof(gname));

	type = xmlGetProp(groupNode,(xmlChar *)"type");
	attID = H5NXget_attribute_string(groupID,gname,"NX_class",data,sizeof(data));
	if(attID < 0){
		NXVsetLog(self,"sev","error");
		NXVsetLog(self,"message","Required group attribute NX_class missing");
		NXVlog(self);
		self->errCount++;
	} else {
		if(strcmp(data,(char *)type) != 0){
			/*
				accept NXsubentry if NXentry is asked for
			*/
			if(!(strcmp((char *)type,"NXentry") == 0 &&
		 	strcmp(data,"NXsubentry") == 0)){
				NXVsetLog(self,"sev","error");
				NXVprintLog(self,"message","Wrong group type, expected %s, got %s",
				(char *)type, data);
				NXVlog(self);
				self->errCount++;
			}
		}
	}
	xmlFree(type);

	/*
		search attribute child nodes
	*/
	cur = groupNode->xmlChildrenNode;
	while(cur != NULL){
		if(xmlStrcmp(cur->name, (xmlChar *)"attribute") == 0){
			name = xmlGetProp(cur,(xmlChar *)"name");
			if(!H5LTfind_attribute(groupID,(char *)name)){
				if(!isOptional(cur)){
					NXVsetLog(self,"sev","error");
					NXVprintLog(self,"message","Required group attribute %s missing",
					name);
					NXVlog(self);
					self->errCount++;
				} else {
					NXVsetLog(self,"sev","warnopt");
					NXVprintLog(self,"message","Optional group attribute %s missing",
					name);
					NXVlog(self);
					self->warnCount++;
				}
			} else {
				/*
					TODO validate attribute data.
					As we do not use group attributes heavily, deferred for
					now
				*/
			}
			xmlFree(name);
		}
		cur = cur->next;
	}
}
/*----------------------------------------------------------------
 Helper stuff for group Finding
 ----------------------------------------------------------------*/
typedef struct {
		char *nxClass;
		char *name;
} findByClassData;
/*--------------------------------------------------------------*/
static int FindByClassIterator(hid_t groupID,
	const char *name,
	const H5L_info_t *info, void *op_data)
{
	findByClassData *fbcb = (findByClassData *)op_data;
	H5O_info_t obj_info;
	hid_t attrID;
	char nxClass[132];

	memset(nxClass,0,sizeof(nxClass));
	H5Oget_info_by_name(groupID, name, &obj_info,H5P_DEFAULT);
	if(obj_info.type == H5O_TYPE_GROUP){
			/*
				work the NX_class attribute
			*/
			attrID = H5NXget_attribute_string(groupID,name,
							  "NX_class", nxClass,sizeof(nxClass));
			if(attrID >= 0){
				if(strcmp(nxClass,fbcb->nxClass) == 0){
					fbcb->name = strdup(name);
					return 1;
				}
			}
  }
	return 0;
}
/*--------------------------------------------------------------
 Finding groups is hideous:
 * They may be specified by name. This seems easy but is complicated
   by the fact that the group name can either be a name attribute or
	 an attribute field.  A design flaw of NXDL, IMHO.
 * They may be specified by type and I need to search by NX_class.
---------------------------------------------------------------*/
static hid_t findGroup(pNXVcontext self, hid_t parentGroup, xmlNodePtr groupNode)
{
	xmlChar *name = NULL, *nxClass = NULL, *nodePath = NULL;
	xmlNodePtr cur = NULL;
	findByClassData fbcd;
	hid_t gid, status;
	hsize_t idx = 0;

  name = xmlGetProp(groupNode,(xmlChar *)"name");
	if(name == NULL){
		cur = groupNode->xmlChildrenNode;
		while(cur != NULL){
			if(xmlStrcmp(cur->name,(xmlChar *)"attribute") == 0){
				name = xmlGetProp(cur,(xmlChar *)"name");
				if(name != NULL){
					break;
				}
			}
			cur = cur->next;
		}
	}
	if(name != NULL){
		if(H5LTpath_valid(parentGroup,(char *)name, 1)){
			status = H5Gopen(parentGroup,(char *)name,H5P_DEFAULT);
		} else {
			status =  -1;
		}
		xmlFree(name);
		return status;
	}

	/*
		no name to be found: search by type
	*/
	nxClass = xmlGetProp(groupNode,(xmlChar *)"type");
	if(nxClass == NULL){
		NXVsetLog(self,"sev","error");
		nodePath = xmlGetNodePath(cur);
		NXVsetLog(self,"nxdlPath", (char *)nodePath);
		NXVsetLog(self,"message","Malformed group entry, type missing");
		NXVlog(self);
		xmlFree(nodePath);
		self->errCount++;
		return -1;
	}

	fbcd.nxClass = (char *)nxClass;
	fbcd.name = NULL;
	H5Literate(parentGroup, H5_INDEX_NAME, H5_ITER_INC, &idx,
		FindByClassIterator, &fbcd);
	if(fbcd.name != NULL){
		gid = H5Gopen(parentGroup,fbcd.name,H5P_DEFAULT);
		free(fbcd.name);
		xmlFree(fbcd.nxClass);
		return gid;
	}
	xmlFree(fbcd.nxClass);


	return -1;
}
/*--------------------------------------------------------------
  This is tricky ans the NXDL target only contains group
	names. I do this by working myself backward from the
	links path and check that all encountered groups have the
	right type. Another complication is that where a NXentry is
	called for, a NXsubentry will do too.
----------------------------------------------------------------*/
static void validateLinkTarget(pNXVcontext self, xmlChar *target,
	char *hdfTarget)
{
	char *nxdlTarget = strdup((char *)target);
	char *pClass = NULL, data[512], *pPtr;
	herr_t attID;

	pPtr = strrchr(hdfTarget,'/');
	*pPtr = '\0';
	pClass = strrchr(nxdlTarget,'/');
	*pClass = '\0';

	pClass = strrchr(nxdlTarget,'/');
	while(pClass != NULL && strlen(pClass) > 1){
		memset(data,0,sizeof(data));
		attID = H5NXget_attribute_string(self->fileID,hdfTarget,"NX_class",
						 data, sizeof(data));
		if(attID < 0){
			free(nxdlTarget);
			NXVsetLog(self,"sev","error");
			NXVprintLog(self,"message","Missing NX_class attribute at %s",
				hdfTarget);
			NXVlog(self);
			self->errCount++;
			return ;
		}
		if(strcmp(data, pClass+1) != 0){
			if(strcmp(pClass+1,"NXentry") != 0 && strcmp(data,"NXsubentry") != 0){
				free(nxdlTarget);
				NXVsetLog(self,"sev","error");
				NXVprintLog(self,"message","Link group mismatch, expected %s, got %s",
					pClass+1, data);
				NXVlog(self);
				self->errCount++;
				return ;
			}
		}
		pPtr = strrchr(hdfTarget,'/');
		*pPtr = '\0';
		*pClass = '\0';
		pClass = strrchr(nxdlTarget,'/');
	}

	free(nxdlTarget);
	return;
}
/*--------------------------------------------------------------*/
static void validateLink(pNXVcontext self, hid_t groupID,
		xmlChar *name, xmlChar *target)
{
	hid_t objID;
	herr_t att;
	char linkTarget[512], dataPath[512];
  char nxdlPath[512], curPath[512];

	/*
		set our path for correct error printing
	*/
	snprintf(nxdlPath,sizeof(nxdlPath),"%s/%s", self->nxdlPath,
		(char *)name);
	NXVsetLog(self,"nxdlPath",nxdlPath);
	H5Iget_name(groupID,curPath,sizeof(curPath));
	snprintf(dataPath,sizeof(dataPath),"%s/%s", curPath,
		(char *)name);
	NXVsetLog(self,"dataPath",dataPath);

	/*
		log what we are doing
	*/
	NXVsetLog(self,"sev","debug");
	NXVprintLog(self,"message","Validating link %s at %s",
		(char *)name, (char *)target);
	NXVlog(self);

	/*
		now we really validate
	*/
	if(H5LTpath_valid(groupID,(char *)name, 1)){
		/*
		  The positive test means that the link exists and is pointing to
			a valid HDF5 object. This is alread good.
		*/
		objID = H5Oopen(groupID,(char *)name,H5P_DEFAULT);
		assert(objID >= 0); /* we just tested for existence, didn't we? */
		memset(linkTarget,0,sizeof(linkTarget));
		att = H5NXget_attribute_string(groupID,(char *)name,"target",
					       linkTarget,sizeof(linkTarget));
		if(att < 0){
				NXVsetLog(self,"sev","error");
				H5Iget_name(objID,dataPath,sizeof(dataPath));
				NXVsetLog(self,"dataPath",dataPath);
				NXVsetLog(self,"message","Link is missing required attribute target");
				NXVlog(self);
				self->errCount++;
		} else {
			/*
				test that the target attribute really points to something
				real. It could be that the link was done right but the
				target attribute set sloppily.
			*/
			if(!H5LTpath_valid(self->fileID,linkTarget,1)){
				NXVsetLog(self,"sev","error");
				H5Iget_name(objID,dataPath,sizeof(dataPath));
				NXVsetLog(self,"dataPath",dataPath);
				NXVprintLog(self,"message","Link target %s is invalid",
					linkTarget);
				NXVlog(self);
				self->errCount++;
			} else {
				validateLinkTarget(self,target,linkTarget);
			}
		}
		H5Oclose(objID);
	} else {
		NXVsetLog(self,"sev","error");
		NXVprintLog(self,"message",
			"Required link %s missing or not pointing to an HDF5 object",
			(char *)name);
		NXVlog(self);
		self->errCount++;
	}
}
/*-------------------------------------------------------------*/
static hid_t findDependentField(pNXVcontext self,
		hid_t inFieldID,char *dpData)
{
	char *pPtr;
	hid_t fieldID, groupID;
	char fname[512], newPath[1024], groupName[1024];

	/*
		get at enclosing group
  */
	memset(groupName,0,sizeof(groupName));
	H5Iget_name(inFieldID,groupName,sizeof(groupName));
	pPtr = strrchr(groupName,'/');
	*pPtr = '\0';
	pPtr = NULL;

	pPtr = strchr(dpData,'/');

	if(pPtr != NULL){
		if(pPtr == dpData){
			/* absolute path */
			if(H5LTpath_valid(self->fileID,dpData,1)){
				fieldID = H5Oopen(self->fileID,dpData,H5P_DEFAULT);
				return fieldID;
			} else {
				return -1;
			}
		} else {
			/* relative path further along the path */
			snprintf(newPath,sizeof(newPath), "%s/%s", groupName, dpData);
			if(H5LTpath_valid(self->fileID,newPath,1)){
				fieldID = H5Oopen(self->fileID,newPath,H5P_DEFAULT);
				return fieldID;
			} else {
				return -1;
			}
		}
	} else {
		/* path within the group */
		groupID = H5Oopen(self->fileID, groupName,H5P_DEFAULT);
		if(H5LTfind_dataset(groupID,dpData)){
			fieldID = H5Dopen(groupID,dpData,H5P_DEFAULT);
			H5Oclose(groupID);
			return fieldID;
		} else {
			H5Oclose(groupID);
			return -1;
		}
	}

	return -1;
}
/*-------------------------------------------------------------*/
static void validateFloat3Attribute(pNXVcontext self,
	hid_t dpField, char *name)
{
	hid_t attID, attType, attSpace;
	H5T_class_t h5class;
	hsize_t dims[2], maxDims[2];
	char fname[512];

	memset(fname,0,sizeof(fname));

	H5Iget_name(dpField,fname,sizeof(fname));

	if(!H5LTfind_attribute(dpField,name)){
			NXVsetLog(self,"sev","error");
			NXVprintLog(self,"message",
				"Missing attribute %s on %s",
				name, fname);
			NXVlog(self);
			self->errCount++;
	} else {
		attID = H5Aopen(dpField,name,H5P_DEFAULT);
		assert(attID >= 0);
		attType = H5Aget_type(attID);
		assert(attType >= 0);
		h5class = H5Tget_class(attType);
		if(h5class != H5T_FLOAT){
			NXVsetLog(self,"sev","error");
			NXVprintLog(self,"message",
				"%s attribute on %s is of wrong type, expected float",
				name, fname);
			NXVlog(self);
			self->errCount++;
		} else {
			attSpace = H5Aget_space(attID);
			if(H5Sget_simple_extent_ndims(attSpace) != 1){
				NXVsetLog(self,"sev","error");
				NXVprintLog(self,"message",
					"%s attribute on %s is of wrong rank, expected 1",
					name, fname);
				NXVlog(self);
				self->errCount++;
			} else {
				H5Sget_simple_extent_dims(attSpace,dims,maxDims);
				if(dims[0] != 3){
					NXVsetLog(self,"sev","error");
					NXVprintLog(self,"message",
						"%s attribute on %s is of wrong size, expected 3",
						name, fname);
					NXVlog(self);
					self->errCount++;
				}
			}
			H5Sclose(attSpace);
		}
		H5Tclose(attType);
		H5Aclose(attID);
	}
}
/*--------------------------------------------------------------
	This validates the lesser depends_on chain fields like
	vector, offset and transformation_type
----------------------------------------------------------------*/
static void validateDependsOnAttributes(pNXVcontext self,hid_t dpField)
{
	char fname[512], transData[512];
	hid_t attID, attType, attSpace;
	H5T_class_t h5class;

	memset(fname,0,sizeof(fname));
	memset(transData,0,sizeof(transData));

	H5Iget_name(dpField,fname,sizeof(fname));

	/*
		deal with transformation_type
	*/
	if(!H5LTfind_attribute(dpField,"transformation_type")){
			NXVsetLog(self,"sev","error");
			NXVprintLog(self,"message",
				"Missing attribute transformation_type on %s",
				fname);
			NXVlog(self);
			self->errCount++;
	} else {
		attID = H5Aopen(dpField,"transformation_type",H5P_DEFAULT);
		assert(attID >= 0);
		attType = H5Aget_type(attID);
		assert(attType >= 0);
		h5class = H5Tget_class(attType);
		if(h5class != H5T_STRING){
			NXVsetLog(self,"sev","error");
			NXVprintLog(self,"message",
				"transformation_type on %s is of wrong type, expected string",
				fname);
			NXVlog(self);
			self->errCount++;
		} else {
			H5NXget_attribute_string(self->fileID, fname,
					"transformation_type",
						 transData,sizeof(transData));
			if(strcmp(transData,"translation") != 0
				&& strcmp(transData,"rotation") != 0){
					NXVsetLog(self,"sev","error");
					NXVprintLog(self,"message",
						"transformation_type on %s contains bad data: %s",
						fname,
						"expected rotation or translation");
					NXVlog(self);
					self->errCount++;
				}
		}
		H5Tclose(attType);
		H5Aclose(attID);
	}

	validateFloat3Attribute(self,dpField,"offset");
	validateFloat3Attribute(self,dpField,"vector");

}
/*--------------------------------------------------------------*/
static void validateDependsOnField(pNXVcontext self,
		hid_t groupID,hid_t dpFieldID)
{
	char fname[512], transData[512];
	hid_t attID, attType, attSpace, dpField;
	H5T_class_t h5class;

	memset(fname,0,sizeof(fname));
	memset(transData,0,sizeof(transData));

	H5Iget_name(dpFieldID,fname,sizeof(fname));
	NXVsetLog(self,"dataPath", fname);
	NXVsetLog(self,"sev","debug");
	NXVprintLog(self,"message","Validating depends_on element %s", fname);
	NXVlog(self);

	/*
		validate the simple ones
	*/
	validateDependsOnAttributes(self,dpFieldID);

	/*
		follow the depends_on chain
	*/
	if(!H5LTfind_attribute(dpFieldID,"depends_on")){
			NXVsetLog(self,"sev","error");
			NXVprintLog(self,"message",
				"Missing attribute depends_on on %s",
				fname);
			NXVlog(self);
			self->errCount++;
	} else {
		attID = H5Aopen(dpFieldID,"depends_on",H5P_DEFAULT);
		assert(attID >= 0);
		attType = H5Aget_type(attID);
		assert(attType >= 0);
		h5class = H5Tget_class(attType);
		if(h5class != H5T_STRING){
			NXVsetLog(self,"sev","error");
			NXVprintLog(self,"message",
				"depends_onattribute on %s is of wrong type, expected string",
				fname);
			NXVlog(self);
			self->errCount++;
		} else {
			H5NXget_attribute_string(self->fileID, fname,
						 "depends_on",transData,sizeof(transData));
			if(strcmp(transData,".") == 0){
				H5Dclose(dpFieldID);
				return;
			} else {
				dpField = findDependentField(self,dpFieldID,transData);
				if(dpField < 0){
						NXVsetLog(self,"sev","error");
						NXVprintLog(self,"message","Dependency chain broken at %s, %s %s",
					 		fname, transData, "pointing nowhere (nirvana?)");
						NXVlog(self);
						self->errCount++;
				} else {
					validateDependsOnField(self,groupID,dpField);
					H5Dclose(dpField);
				}
			}
		}
	}
}
/*--------------------------------------------------------------*/
static void validateDependsOn(pNXVcontext self, hid_t groupID,
	hid_t fieldID)
{
	char fname[512], dpData[1024];
	hid_t h5type, dpfieldID;
	H5T_class_t h5class;

	memset(fname,0,sizeof(fname));
	memset(dpData,0,sizeof(dpData));
	H5Iget_name(fieldID,fname,sizeof(fname));
	NXVsetLog(self,"dataPath",fname);
	NXVsetLog(self,"sev","debug");
	NXVprintLog(self,"message","Validating depends_on chain starting at %s",
						fname);
	NXVlog(self);

	/*
		test that the depends_on field is of the right type
	*/
	h5type = H5Dget_type(fieldID);
	h5class = H5Tget_class(h5type);
	H5Tclose(h5type);
	if(h5class != H5T_STRING){
			NXVsetLog(self,"sev","error");
			NXVsetLog(self,"message",
			"depends_on field is of wrong type, expect string");
			NXVlog(self);
			self->errCount++;
			return;
	}

	/*
		read the field
	*/
	H5NXread_dataset_string(groupID,"depends_on",dpData,sizeof(dpData));
	/*
		find the field and start iterating through the chain
	*/

        if (strcmp(dpData,".")!=0) { 
	dpfieldID = findDependentField(self,fieldID,dpData);
	if(dpfieldID < 0){
		NXVsetLog(self,"sev","error");
		NXVprintLog(self,"message",
		"Cannot even find the starting point of the depends_on chain, %s",
		dpData);
		NXVlog(self);
		self->errCount++;
		return;
	} else {
		validateDependsOnField(self,groupID,dpfieldID);
		H5Dclose(dpfieldID);
	}
	}

}
/*---------------------------------------------------------------
For group validationwe  need two passes:

* In the first pass we try to find all the stuff in the NXDL
  group
* In the second pass we scan the HDF5 group in order to
  locate extra stuff

I keep the names already seen in the first pass in a
hash table

----------------------------------------------------------------*/
typedef struct {
	hash_table *namesSeen;
	hash_table *baseNames;
	pNXVcontext self;
} SecondPassData;
/*--------------------------------------------------------------*/
static herr_t SecondPassIterator(hid_t g_id,
		const char *name,
		const H5L_info_t *info, void *op_data)
{
	SecondPassData *spd = (SecondPassData *)op_data;
  H5O_info_t obj_info;
	hid_t attID, groupID, dataID;
	char nxClass[512], fname[512];

	/*
		have we seen that yet?
	*/
	if(hash_lookup((char *)name,spd->namesSeen) != NULL){
		return 0;
	}

	/*
		Nope, we will have to warn...
	*/
	H5Oget_info_by_name(g_id, name, &obj_info,H5P_DEFAULT);
	if(obj_info.type == H5O_TYPE_GROUP){
		groupID = H5Gopen(g_id,name,H5P_DEFAULT);
		H5Iget_name(groupID, fname,sizeof(fname));
		NXVsetLog(spd->self,"dataPath",fname);
		if(H5LTfind_attribute(groupID,"NX_class") == 1){
			memset(nxClass,0,sizeof(nxClass));
			H5NXget_attribute_string(g_id,name,
						 "NX_class", nxClass,sizeof(nxClass));
			if(hash_lookup(nxClass,spd->baseNames) == NULL){
				NXVsetLog(spd->self,"sev","warnundef");
				NXVprintLog(spd->self,"message","Unknown group %s of class %s found",
					name, nxClass);
				NXVlog(spd->self);
				spd->self->warnCount++;
			} else {
				NXVsetLog(spd->self,"sev","warnbase");
				NXVprintLog(spd->self,"message",
					"Additional base class group %s of type %s found",
					name, nxClass);
				NXVlog(spd->self);
				spd->self->warnCount++;
			}
		} else {
			NXVsetLog(spd->self,"sev","warnundef");
			NXVprintLog(spd->self,"message",
				"Additional non NeXus group %s found",
				name);
			NXVlog(spd->self);
			spd->self->warnCount++;
    }
		H5Gclose(groupID);
	} else if (obj_info.type == H5O_TYPE_DATASET) {
		dataID = H5Dopen(g_id,name,H5P_DEFAULT);
		H5Iget_name(dataID, fname,sizeof(fname));
		H5Dclose(dataID);
		NXVsetLog(spd->self,"dataPath",fname);
		if(hash_lookup((char *)name,spd->baseNames) == NULL){
			NXVsetLog(spd->self,"sev","warnundef");
			NXVprintLog(spd->self,"message","Unknown dataset %s found",
			name);
			NXVlog(spd->self);
			spd->self->warnCount++;
		} else {
			NXVsetLog(spd->self,"sev","warnbase");
			NXVprintLog(spd->self,"message","Additional base class dataset %s found",
			name);
			NXVlog(spd->self);
			spd->self->warnCount++;
		}
	}

	return 0;
}
/*--------------------------------------------------------------*/
int NXVvalidateGroup(pNXVcontext self, hid_t groupID,
	xmlNodePtr groupNode)
{
		hash_table namesSeen, baseNames;
		xmlNodePtr cur = NULL;
		xmlChar *name = NULL, *myClass = NULL;
		xmlChar *target = NULL;
		hid_t childID;
		char fName[256], childName[512], nxdlChildPath[512], childPath[512];
		char mynxdlPath[512];
		char *savedNXDLPath, *pPtr;
		SecondPassData spd;
		hsize_t idx = 0;

		/*
			manage nxdlPath, xmlGetNodePath does not work
		*/
		savedNXDLPath = self->nxdlPath;
		myClass = xmlGetProp(groupNode,(xmlChar *)"type");
		if(self->nxdlPath == NULL) {
			snprintf(mynxdlPath,sizeof(mynxdlPath),"/%s", (char *) myClass);
		} else {
			snprintf(mynxdlPath,sizeof(mynxdlPath),"%s/%s",
				self->nxdlPath, (char *) myClass);
		}
		self->nxdlPath = mynxdlPath;

		/*
			tell what we are doing
		*/
		H5Iget_name(groupID,fName,sizeof(fName));
		NXVsetLog(self,"sev","debug");
		NXVsetLog(self,"message","Validating group");
		NXVsetLog(self,"nxdlPath",self->nxdlPath);
		NXVsetLog(self,"dataPath",fName);
		NXVlog(self);


		validateGroupAttributes(self, groupID, groupNode);
		hash_construct_table(&namesSeen,100);

		/* first pass */
		cur = groupNode->xmlChildrenNode;
		while(cur != NULL){
			if(xmlStrcmp(cur->name,(xmlChar *) "group") == 0){
					childID = findGroup(self, groupID, cur);
					if(childID >= 0){
							H5Iget_name(childID, childName,sizeof(childName));
							/*
								we have to get at the HDF5 name. There may be no
								name in the NXDL, but a suitable group has been found
								by NXclass.
							*/
							pPtr = strrchr(childName,'/');
							if(pPtr != NULL){
								hash_insert(pPtr+1,strdup(""),&namesSeen);
							} else {
								hash_insert(childName,strdup(""),&namesSeen);
							}
							NXVvalidateGroup(self,childID,cur);
					} else {
						name = xmlGetProp(cur,(xmlChar *)"type");
						snprintf(nxdlChildPath,sizeof(nxdlChildPath),"%s/%s",
							self->nxdlPath, (char *)name);
						xmlFree(name);
						NXVsetLog(self,"dataPath",fName);
						NXVsetLog(self,"nxdlPath", nxdlChildPath);
						if(!isOptional(cur)){
							NXVsetLog(self,"sev","error");
							NXVsetLog(self,"message","Required group missing");
							NXVlog(self);
							self->errCount++;
						} else {
							NXVsetLog(self,"sev","warnopt");
							NXVsetLog(self,"message","Optional group missing");
							NXVlog(self);
							self->warnCount++;
						}
					}
			}
			if(xmlStrcmp(cur->name,(xmlChar *) "field") == 0){
					name = xmlGetProp(cur,(xmlChar *)"name");
					if(H5LTfind_dataset(groupID,(char *)name) ) {
						childID = H5Dopen(groupID,(char *)name,H5P_DEFAULT);
					} else {
						childID = -1;
					}
					snprintf(childPath,sizeof(childPath),"%s/%s",
						fName,name);
					if(childID < 0){
						NXVsetLog(self,"dataPath",childPath);
						snprintf(nxdlChildPath,sizeof(nxdlChildPath),
							"%s/%s", self->nxdlPath, name);
						NXVsetLog(self,"nxdlPath", nxdlChildPath);
						if(!isOptional(cur)){
							NXVsetLog(self,"sev","error");
							NXVsetLog(self,"message","Required field missing");
							NXVlog(self);
							self->errCount++;
						} else {
							NXVsetLog(self,"sev","warnopt");
							NXVsetLog(self,"message","Optional field missing");
							NXVlog(self);
							self->warnCount++;
						}
					} else {
						if(xmlStrcmp(name,(xmlChar *)"depends_on") == 0){
							/*
								This must b validated from the field level. As
								it might point to fields which are not in the
								application definition
							*/
							validateDependsOn(self,groupID,childID);
						} else {
							NXVvalidateField(self,childID, cur);
						}
						hash_insert((char *)name,strdup(""),&namesSeen);
					}
					xmlFree(name);
			}
			if(xmlStrcmp(cur->name,(xmlChar *) "link") == 0){
				name = xmlGetProp(cur,(xmlChar *)"name");
				target = xmlGetProp(cur,(xmlChar *)"target");
				hash_insert((char *)name,strdup(""),&namesSeen);
				validateLink(self,groupID,name, target);
				xmlFree(name);
				xmlFree(target);
			}
			cur = cur->next;
		}

		/*
			Second pass: search the HDF5 group for additional
			stuff which have not checked yet. Most of the hard work
			is in the SecondPassIterator.
		*/
		hash_construct_table(&baseNames,100);
		NXVloadBaseClass(self,&baseNames,(char *)myClass);
		spd.baseNames = &baseNames;
		spd.namesSeen = &namesSeen;
		spd.self = self;
		NXVsetLog(self,"nxdlPath", mynxdlPath);
		H5Literate(groupID, H5_INDEX_NAME, H5_ITER_INC, &idx,
			SecondPassIterator, &spd);

		/*
			clean up
		*/
		hash_free_table(&namesSeen,free);
		hash_free_table(&baseNames,free);
		xmlFree(myClass);
		/*
			restore my paths...
		*/
		self->nxdlPath = savedNXDLPath;
		return 0;
	}
