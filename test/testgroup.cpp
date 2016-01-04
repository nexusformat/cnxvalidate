/**
  * This file contains tests for the group checking
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

SCENARIO("Verify no errors for OK group elements","[nogrouperror]"){
  GIVEN("An initialized validation context"){
    pNXVcontext con = NXVinit("data");
    multimap<string,string> *testResult = prepareTest(con);
    REQUIRE(con != NULL);

    WHEN("Validating group.h5"){
      int status = NXVvalidate(con,"data/group.h5", NULL, NULL);

      THEN("Path /entry/title should not be in errors"){
        REQUIRE(testNoError(testResult,"/entry/title"));
      }
      THEN("Path /entry/definition should not be in errors"){
        REQUIRE(testNoError(testResult,"/entry/definition"));
      }
      THEN("Path /entry/data should not be in errors"){
        REQUIRE(testNoError(testResult,"/entry/data"));
      }
    }
  }
}

SCENARIO("Warning for additional base class elements","[addbase]"){
  GIVEN("An initialized validation context"){
    pNXVcontext con = NXVinit("data");
    multimap<string,string> *testResult = prepareTest(con);
    REQUIRE(con != NULL);

    WHEN("Validating group.h5"){
      int status = NXVvalidate(con,"data/group.h5", NULL, NULL);

      THEN("Path /entry/experiment_description should warn additional base class field"){
        REQUIRE(findMessage(testResult,"/entry/experiment_description",
          "warnbase","Additional base class dataset"));
      }
      THEN("Path /entry/sample should warn additional base class group"){
        REQUIRE(findMessage(testResult,"/entry/sample",
          "warnbase","Additional base class group"));
      }
    }
  }
}

SCENARIO("Warning for additional non base class elements","[nonbase]"){
  GIVEN("An initialized validation context"){
    pNXVcontext con = NXVinit("data");
    multimap<string,string> *testResult = prepareTest(con);
    REQUIRE(con != NULL);

    WHEN("Validating group.h5"){
      int status = NXVvalidate(con,"data/group.h5", NULL, NULL);

      THEN("Path /entry/experiment_ingredients should warn additional non base class field"){
        REQUIRE(findMessage(testResult,"/entry/experiment_ingredients",
          "warnundef","Unknown dataset"));
      }
      THEN("Path /entry/wool should warn additional non base class group"){
        REQUIRE(findMessage(testResult,"/entry/wool",
          "warnundef","Unknown group"));
      }
    }
  }
}

SCENARIO("Warning about non NeXus group","[nonnexusgroup]"){
  GIVEN("An initialized validation context"){
    pNXVcontext con = NXVinit("data");
    multimap<string,string> *testResult = prepareTest(con);
    REQUIRE(con != NULL);

    WHEN("Validating group.h5"){
      int status = NXVvalidate(con,"data/group.h5", NULL, NULL);
      THEN("Path /entry/missing_class should warn about non NeXus group"){
        REQUIRE(findMessage(testResult,"/entry/missing_class",
          "warnundef","Additional non NeXus group"));
      }
    }
  }
}

SCENARIO("Finding a group specified by type in NXDL","[findByType]"){
  GIVEN("An initialized validation context"){
    pNXVcontext con = NXVinit("data");
    multimap<string,string> *testResult = prepareTest(con);
    REQUIRE(con != NULL);

    WHEN("Validating group.h5"){
      int status = NXVvalidate(con,"data/group.h5", NULL, NULL);
      THEN("Path /entry/data/data_sample should be validated"){
        REQUIRE(findMessage(testResult,"/entry/data/data_sample",
          "debug","Validating group"));
      }
    }
  }
}
