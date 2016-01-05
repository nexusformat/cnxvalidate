/**
  * This file contains tests for the link checking
	* parts of nxvalidate
	*
	* copyright: GPL
	*
	* Mark Koennecke, mark.koennecke@psi.ch, and NIAC
	* January 2016
	*/
  #include "catch.hpp"
  #include <nxvalidate.h>
  #include "testutil.hpp"

  SCENARIO("No error when link OK","[linkok]"){
    GIVEN("An initialized validation context"){
      pNXVcontext con = NXVinit("data");
      multimap<string,string> *testResult = prepareTest(con);
      REQUIRE(con != NULL);

      WHEN("Validating link.h5"){
        int status = NXVvalidate(con,"data/link.h5", NULL, NULL);

        THEN("Path /entry/title should not be in error"){
          REQUIRE(testNoError(testResult,"/entry/title"));
        }
      }
    }
  }

  SCENARIO("Error when target attribute is pointing nowhere","[linkbad]"){
    GIVEN("An initialized validation context"){
      pNXVcontext con = NXVinit("data");
      multimap<string,string> *testResult = prepareTest(con);
      REQUIRE(con != NULL);

      WHEN("Validating link.h5"){
        int status = NXVvalidate(con,"data/link.h5", NULL, NULL);

        THEN("Path /entry/description should cause an error: bad target"){
          REQUIRE(findMessage(testResult,"/entry/description",
            "error","Link target /target/guggelhupf is invalid"));
        }
      }
    }
  }

  SCENARIO("Error when target attribute is missing","[linkmis]"){
    GIVEN("An initialized validation context"){
      pNXVcontext con = NXVinit("data");
      multimap<string,string> *testResult = prepareTest(con);
      REQUIRE(con != NULL);

      WHEN("Validating link.h5"){
        int status = NXVvalidate(con,"data/link.h5", NULL, NULL);

        THEN("Path /entry/revision should cause an error: missing target attribute"){
          REQUIRE(findMessage(testResult,"/entry/revision",
            "error","Link is missing required attribute target"));
        }
      }
    }
  }

  SCENARIO("Error when the HDF5 link is bad","[badh5link]"){
    GIVEN("An initialized validation context"){
      pNXVcontext con = NXVinit("data");
      multimap<string,string> *testResult = prepareTest(con);
      REQUIRE(con != NULL);

      WHEN("Validating link.h5"){
        int status = NXVvalidate(con,"data/link.h5", NULL, NULL);

        THEN("Path /entry/hugo should cause an error: invalid link"){
          REQUIRE(findMessage(testResult,"/entry/hugo",
            "error","Required link hugo missing or not pointing to an HDF5 object"));
        }
      }
    }
  }

  SCENARIO("Error when link object is missing alltogether","[nolink]"){
    GIVEN("An initialized validation context"){
      pNXVcontext con = NXVinit("data");
      multimap<string,string> *testResult = prepareTest(con);
      REQUIRE(con != NULL);

      WHEN("Validating link.h5"){
        int status = NXVvalidate(con,"data/link.h5", NULL, NULL);

        THEN("Path /entry/fehlt should cause an error: missing link"){
          REQUIRE(findMessage(testResult,"/entry/fehlt",
            "error","Required link fehlt missing or not pointing to an HDF5 object"));
        }
      }
    }
  }
