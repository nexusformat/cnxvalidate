/**
* This module implements all functions and utilities which have to
* do with the validation context.
*
* Copyright: GPL
*
* Mark Koennecke, mark.koennecke@psi.ch, and NIAC
* November 2015
*/

#include <nxvalidate.h>
#include <hdf5.h>
#include <libxml/tree.h>

typedef struct __NXVContext {
		char *dataFile; /* name of the data file we are validating */
		char *nxdlFile; /* name of the nxdl file we are validating against */
		char *nxdlPath; /* The path in the NXDL file where we are now */
		char *validationRoot; /* the entry or subentry we are validating, needed for
		                         doing depends_on validation */
		validateLogger logger; /* the logger to use and its use rdata */
		void *loggerUserData;
		RetrieveNXDL   nxdlRetriever; /* the nxdl retriever to use and its
		                                 user data*/
		void *retrieverUserData;
		hash_table logData; /* The hashtable for holding logging data */
		hid_t fileID;   /* the HDF5 fileID */
		int subEntryFound;  /* Needed when iteratinhg for subEntries */
		unsigned int errCount;
		unsigned int warnCount;
		xmlDocPtr nxdlDoc;  /* The XML document against we are validating */
		hash_table dimSymbols; /* The symbol table for dimensions symbols */
}NXVcontext;

/*
	* reinitialize the context for doing another file
	*/
void NXVprepareContext(pNXVcontext self, char *dataFile, char *appDef);

/*
 * write a log entry
 */
void NXVlog(pNXVcontext self);

/*
 * set an entry in the log dictionary
 */
void NXVsetLog(pNXVcontext self, char *key, char *value);
void NXVprintLog(pNXVcontext self, char *key, char *format, ...);

/*
 * implemented in nxdlutil.c
 */
int NXVloadAppDef(pNXVcontext self, char *nxdlFile);
xmlNodePtr NXVfindEntry(pNXVcontext self);
void NXVloadBaseClass(pNXVcontext self,
		hash_table *baseNames, char *baseClass);

/*
 * implementd in nxvgroup.c
 */
int NXVvalidateGroup(pNXVcontext self, hid_t groupID,
	xmlNodePtr groupNode);

/*
 * implemented in nxvfield.c
 */
 int NXVvalidateField(pNXVcontext self, hid_t fieldID,
 	xmlNodePtr groupNode);

/*
 * implemented in nxvmainloop.c
 */
int testISO8601(char *date);
