/**
  * This header file contains utility functions for
	* testing cnxvalidate
	*
	* Copyright: GPL
	*
	* Mark Koennecke, mark.koennecke@psi.ch, and NIAC
	* December 2015
	*/
#ifndef __TESTUTIL
#define __TESTUTIL
#include <map>

using namespace std;

/**
	* install the special test log write which stores
	* responses in map using the datapath as a key
	* \param con The context to configure
	* \return A multimap which will contain the log of
	* the nxvalidate run after running NXVvalidate.
	*/
multimap<string,string> *prepareTest(pNXVcontext con);
/**
  * try to find the specified message on dataPath
  * \param result, The result dictionary of the run
  * \param dataPath The dataPath where we expect the message
  * \param severity The severity of the message
  * \param message The message text
  * \return 1 when message found, 0 else
  */
int findMessage(multimap<string,string> *result,
  string dataPath,string severity, string message);

/**
  * verify that no error has been found for datapath
  * \param result The result dictionary of the run
  * \param dataPath The dataPath to checking
  * \param 1 when no error found, 0 when there is an error
  */
int testNoError(multimap<string,string> *result,string dataPath);

#endif
