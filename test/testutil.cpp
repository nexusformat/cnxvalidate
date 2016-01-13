/**
  * This  implementation file contains utility functions for
	* testing cnxvalidate
	*
	* Copyright: GPL
	*
	* Mark Koennecke, mark.koennecke@psi.ch, and NIAC
	* December 2015
	*/
#include <string.h>
#include <iostream>
#include <nxvalidate.h>
#include "testutil.hpp"

static string slogData;
/*--------------------------------------------------------------
 This is not thread safe due to logData. But that hash_enumerate
 lacks a means to pass in a pointer to your own stuf..
----------------------------------------------------------------*/
static void storeLog(char *key, void *data)
{
	if(strcmp(key,"dataPath") == 0){
		return;
	}
	if(strchr((char *)data, ' ') != NULL){
		slogData.append(key);
		slogData.append("=\"");
		slogData.append((char *)data);
		slogData.append("\"");
	} else {
		slogData.append(key);
		slogData.append("=");
		slogData.append((char *)data);
	}
	slogData.append(" ");
}

/*--------------------------------------------------------------*/
static void TestLogger(hash_table *logData, void *userData)
{
	multimap<string,string> *resultMap = (multimap<string,string> *)userData;
	char *dataPath;

	dataPath = (char *)hash_lookup("dataPath",logData);
	if(dataPath == NULL){
		dataPath = strdup("/");
	}

	slogData = string("");
	hash_enumerate(logData,storeLog);
	resultMap->insert(pair<string,string>(dataPath, slogData));
	/*
		This is useful for debugging tests but should be out in the
		end.
	*/
	//cout << dataPath << ":" <<slogData << "\n";

}

/*-----------------------------------------------------*/
multimap<string,string> *prepareTest(pNXVcontext con)
{
	multimap<string,string> *result = new multimap<string,string>();
	NXVsetLogger(con,TestLogger,result);

	return result;
}
/*-----------------------------------------------------*/
int findMessage(multimap<string,string> *result,
  string dataPath,string severity, string message)
{

	pair <multimap<string,string>::iterator, multimap<string,string>::iterator> ret;
	multimap<string,string>::iterator it;

	ret = result->equal_range(dataPath);
	for(it = ret.first; it != ret.second; ++it){
			string test = it->second;
			string sev = "sev=" + severity;
			if(test.find(sev,0) != string::npos &&
			test.find(message,0) != string::npos){
				return 1;
			}
	}
	return 0;
}
/*-----------------------------------------------------*/
int testNoError(multimap<string,string> *result,string dataPath)
{
	pair <multimap<string,string>::iterator, multimap<string,string>::iterator> ret;
	multimap<string,string>::iterator it;
	string sev = "sev=debug";

	ret = result->equal_range(dataPath);
	for(it = ret.first; it != ret.second; ++it){
		string test = it->second;
		if(test.find(sev,0) == string::npos) {
			return 0;
		}
	}
	return 1;
}
