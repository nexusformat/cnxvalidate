/**
  * This file implements some utilities for processing application
	* definitions. Most importantly loading and flattening the
	* inheritance hierachy.
	*
	* copyright: GPL
  *
	* Mark Koennecke, mark.koennecke@psi.ch, and NIAC
	*/
#include "nxvcontext.h"
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <string.h>

/*----------------------------------------------------*/
static xmlChar *findGroupName(xmlNodePtr group)
{
	xmlChar *name;
	xmlNodePtr cur;

	name = xmlGetProp(group,(xmlChar *)"name");
	if(name != NULL){
		return name;
	}

	cur = group->xmlChildrenNode;
	while(cur != NULL){
		if(xmlStrcmp(cur->name,(xmlChar *)"attribute") == 0){
			name = xmlGetProp(cur,(xmlChar *)"name");
			if(name != NULL){
				return name;
			}
			xmlFree(name);
		}
		cur = cur->next;
	}
	return NULL;
}
/*----------------------------------------------------*/
static xmlNodePtr xmlFindGroup(xmlNodePtr root,
	xmlChar *type, xmlChar *name)
{
	xmlNodePtr cur;
	xmlChar *myType, *myName;

	cur = root->xmlChildrenNode;
	while(cur != NULL){
			if(xmlStrcmp(cur->name,(xmlChar *)"group") == 0){
				myType = xmlGetProp(cur,(xmlChar *)"type");
				if(xmlStrcmp(myType,type) == 0){
					if(name != NULL){
							myName = findGroupName(cur);
							if(xmlStrcmp(name,myName) == 0){
								xmlFree(myName);
								xmlFree(myType);
								return cur;
							}
					} else {
						xmlFree(myType);
						return cur;
					}
				}
		}
			cur = cur->next;
	}
	return NULL;
}
/*----------------------------------------------------*/
static xmlNodePtr findField(xmlNodePtr root, xmlChar *name)
{
	xmlNodePtr cur;
	xmlChar *myName;

	cur = root->xmlChildrenNode;
	while(cur != NULL){
		if(xmlStrcmp(cur->name,(xmlChar *)"field") == 0){
					myName = xmlGetProp(cur,(xmlChar*)"name");
					if(xmlStrcmp(myName,name) == 0){
						xmlFree(myName);
						return cur;
					}
					xmlFree(myName);
		}
		cur = cur->next;
	}
	return NULL;
}
/*----------------------------------------------------*/
static xmlNodePtr findLink(xmlNodePtr root, xmlChar *name)
{
	xmlNodePtr cur;
	xmlChar *myName;

	cur = root->xmlChildrenNode;
	while(cur != NULL){
		if(xmlStrcmp(cur->name,(xmlChar *)"link") == 0){
					myName = xmlGetProp(cur,(xmlChar*)"name");
					if(xmlStrcmp(myName,name) == 0){
						xmlFree(myName);
						return cur;
					}
					xmlFree(myName);
		}
		cur = cur->next;
	}
	return NULL;
}
/*----------------------------------------------------*/
static void mergeGroups(xmlNodePtr root, xmlNodePtr base)
{
	xmlNodePtr cur, rootCur;
	xmlChar *type, *name;


	cur = base->xmlChildrenNode;
	while(cur != NULL){
		if(xmlStrcmp(cur->name,(xmlChar *)"group") == 0){
				type = xmlGetProp(cur,(xmlChar *)"type");
				name = findGroupName(cur);
				rootCur = xmlFindGroup(root, type, name);
				if(rootCur != NULL){
					mergeGroups(rootCur,cur);
				} else {
					xmlAddChild(root,xmlCopyNode(cur,1));
				}
				xmlFree(type);
				xmlFree(name);
		}
		if(xmlStrcmp(cur->name,(xmlChar *)"field") == 0){
			name = xmlGetProp(cur,(xmlChar *)"name");
			rootCur = findField(root,name);
			if(rootCur == NULL){
				xmlAddChild(root,xmlCopyNode(cur,1));
			}
			xmlFree(name);
		}
		if(xmlStrcmp(cur->name,(xmlChar *)"link") == 0){
			name = xmlGetProp(cur,(xmlChar *)"name");
			rootCur = findLink(root,name);
			if(rootCur == NULL){
				xmlAddChild(root,xmlCopyNode(cur,1));
			}
			xmlFree(name);
		}
		cur = cur->next;
	}
}
/*----------------------------------------------------*/
static void mergeDefinitions(pNXVcontext self,
		xmlNodePtr root, xmlNodePtr base )
{
	xmlChar *attr = NULL;

	if(xmlStrcmp(base->name,(xmlChar *)"definition") !=0 ){
		NXVsetLog(self,"sev","error");
		NXVsetLog(self,"message",
			"No definition element in parent application definition, INVALID");
		NXVlog(self);
		self->errCount++;
		xmlSetProp(root,
			(xmlChar *)"extends",(xmlChar *)"NXobject");
		return;
	}

	/*
	copy extends attribute from base to root in order
	to stop recursion in mergeInheritance
	*/
  attr = xmlGetProp(base,(xmlChar *)"extends");
	xmlSetProp(root,(xmlChar *)"extends",attr);

	mergeGroups(root,base);

}
/*---------------------------------------------------*/
static void mergeInheritance(pNXVcontext self)
{
	xmlNodePtr root = xmlDocGetRootElement(self->nxdlDoc);
	xmlNodePtr cur;
	xmlChar *attr = NULL;
	char fullNXDLName[256];
	char *baseClassData = NULL;
	xmlDocPtr baseNxdl = NULL;
	xmlNodePtr base = NULL;

	if(xmlStrcmp(root->name,(xmlChar *)"definition" ) != 0){
		NXVsetLog(self,"sev","fatal");
		NXVsetLog(self,"message",
			"No definition element in application definition, INVALID");
		NXVlog(self);
		self->errCount++;
		return;
	}

	attr = xmlGetProp(root,(xmlChar *)"extends");
	if(attr == NULL){
		NXVsetLog(self,"sev","error");
		NXVsetLog(self,"message",
			"definition element has no extends attribute, MALFORMED");
		NXVlog(self);
		self->errCount++;
		return;
	}

	if(xmlStrcmp(attr,(xmlChar *)"NXobject") == 0) {
		/* nothing to do */
		return;
	}

  NXVsetLog(self,"sev","debug");
	NXVprintLog(self,"message","Loading parent definition %s",
		(char *)attr);
	NXVlog(self);

	snprintf(fullNXDLName, sizeof(fullNXDLName),"%s.nxdl.xml",
		(char *)attr);
	baseClassData = self->nxdlRetriever(fullNXDLName,self->retrieverUserData);
	if(baseClassData == NULL){
		NXVsetLog(self,"sev","error");
		NXVsetLog(self,"message","Failed to load parent application definition");
		NXVlog(self);
		self->errCount++;
		return;
	}
	baseNxdl = xmlParseDoc((xmlChar *)baseClassData);
	base= xmlDocGetRootElement(baseNxdl);
	if(base == NULL){
		xmlFreeDoc(baseNxdl);
		NXVsetLog(self,"sev","error");
		NXVsetLog(self,"message","Failed to parse parent application definition");
		NXVlog(self);
		self->errCount++;
		return;
	}

	mergeDefinitions(self,root,base);
	xmlFreeDoc(baseNxdl);
	free(baseClassData);

	mergeInheritance(self);

}
/*--------------------------------------------------------------*/
int NXVloadAppDef(pNXVcontext self, char *nxdlFile)
{
	char *pPtr = NULL;
  char *xmlData = NULL;
	xmlNodePtr root = NULL;
  /*
	 get at the raw name without any extras. Such as that the
	 retriever can do her job properly
	 */
	pPtr = strrchr(nxdlFile,'/');
	if(pPtr == NULL){
		pPtr = nxdlFile;
	}

  xmlData = self->nxdlRetriever(pPtr,self->retrieverUserData);
	if(xmlData == NULL){
		NXVsetLog(self,"sev","fatal");
		NXVsetLog(self,"message","Failed to load application definition");
		NXVlog(self);
		self->errCount++;
		return 1;
	}

	if(self->nxdlDoc != NULL){
		xmlFreeDoc(self->nxdlDoc);
	}
	self->nxdlDoc = xmlParseDoc((xmlChar *)xmlData);
	root = xmlDocGetRootElement(self->nxdlDoc);
	if(root == NULL){
		xmlFreeDoc(self->nxdlDoc);
		self->nxdlDoc = NULL;
		NXVsetLog(self,"sev","fatal");
		NXVsetLog(self,"message","Failed to parse application definition");
		NXVlog(self);
		self->errCount++;
		return 1;
	}

  mergeInheritance(self);

  /*
		This is for debugging the inheritance merging
		comment out in production
		*/
	xmlSaveFormatFile("debugxml.xml",self->nxdlDoc,1);
	return 0;
}
/*----------------------------------------------------------------*/
xmlNodePtr NXVfindEntry(pNXVcontext self)
{
	xmlNodePtr root = xmlDocGetRootElement(self->nxdlDoc);
	xmlNodePtr cur;

	if(xmlStrcmp(root->name,(xmlChar *)"definition" ) != 0){
		NXVsetLog(self,"sev","fatal");
		NXVsetLog(self,"message",
			"No definition element in application definition, INVALID");
		NXVlog(self);
		self->errCount++;
		return NULL;
	}

  cur = root->xmlChildrenNode;
	while(cur != NULL){
		if(xmlStrcmp(cur->name,(xmlChar *) "group") == 0){
			return cur;
		}
		cur = cur->next;
	}

	/*
		not found a group element
	*/
	NXVsetLog(self,"sev","fatal");
	NXVsetLog(self,"message",
		"No group element in application definition, INVALID");
	NXVlog(self);
	self->errCount++;
	return NULL;
}
/*--------------------------------------------------------------*/
void NXVloadBaseClass(pNXVcontext self,
		hash_table *baseNames, char *baseClass)
{
	char *baseData;
	char fullBaseClassName[512];
	xmlDocPtr baseDoc;
	xmlNodePtr root, cur;
	xmlChar *data;

	snprintf(fullBaseClassName,sizeof(fullBaseClassName),
		"%s.nxdl.xml", baseClass);

	baseData = self->nxdlRetriever(fullBaseClassName,self->retrieverUserData);
	if(baseData == NULL){
		NXVsetLog(self,"sev","error");
		NXVprintLog(self,"message","Failed to load base class %s", baseClass);
		NXVlog(self);
		self->errCount++;
		return;
	}

	baseDoc = xmlParseDoc((xmlChar *)baseData);
	root = xmlDocGetRootElement(baseDoc);
	if(root == NULL){
		NXVsetLog(self,"sev","error");
		NXVprintLog(self,"message","Failed to parse base class %s", baseClass);
		NXVlog(self);
		self->errCount++;
		return;
	}

	/*
		collect the names from the base class
	*/
	if(xmlStrcmp(root->name,(xmlChar *)"definition" ) != 0){
		NXVsetLog(self,"sev","error");
		NXVsetLog(self,"message",
			"No definition element in base class definition, INVALID");
		NXVlog(self);
		self->errCount++;
		xmlFreeDoc(baseDoc);
		free(baseData);
		return;
	}

	cur = root->xmlChildrenNode;
	while(cur != NULL){
		if(xmlStrcmp(cur->name,(xmlChar *)"group") == 0){
			data = xmlGetProp(cur,(xmlChar *)"type");
			if(data != NULL){
				hash_insert(data,strdup(""),baseNames);
				xmlFree(data);
			}
		}
		if(xmlStrcmp(cur->name,(xmlChar *)"field") == 0){
			data = xmlGetProp(cur,(xmlChar *)"name");
			if(data != NULL){
				hash_insert(data,strdup(""),baseNames);
				xmlFree(data);
			}
		}
		cur = cur->next;
	}

	xmlFreeDoc(baseDoc);
	free(baseData);
}
