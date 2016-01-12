/**
  * This file contains tests for the depends_on chain
	* validation of nxvalidate
	*
	* copyright: GPL
	*
	* Mark Koennecke, mark.koennecke@psi.ch, and NIAC
	* January 2015
	*/
#include "catch.hpp"
#include <nxvalidate.h>
#include "testutil.hpp"

SCENARIO("Testing a valid depends_on chain causing no errors ","[depok]"){
  GIVEN("An initialized validation context"){
    pNXVcontext con = NXVinit("data");
    multimap<string,string> *testResult = prepareTest(con);
    REQUIRE(con != NULL);

    WHEN("Validating depends.h5"){
      int status = NXVvalidate(con,"data/depends.h5", NULL, NULL);

      THEN("Path /good/sample/transform/depends_on should not be in error"){
        REQUIRE(testNoError(testResult,"/good/sample/transform/depends_on"));
      }
			THEN("Path /good/sample/transform/x should not be in error"){
        REQUIRE(testNoError(testResult,"/good/sample/transform/x"));
      }
			THEN("Path /good/sample/transform/z should not be in error"){
        REQUIRE(testNoError(testResult,"/good/sample/transform/z"));
      }
			THEN("Path /good/sample/rotation_angle should not be in error"){
        REQUIRE(testNoError(testResult,"/good/sample/rotation_angle"));
      }
    }
  }
}
SCENARIO("Testing for failure when depends_on field points nowhere ","[depfieldbad]"){
  GIVEN("An initialized validation context"){
    pNXVcontext con = NXVinit("data");
    multimap<string,string> *testResult = prepareTest(con);
    REQUIRE(con != NULL);

    WHEN("Validating depends.h5"){
      int status = NXVvalidate(con,"data/depends.h5", NULL, NULL);
			THEN("Path /startwrong/sample/transform/depends_on should report bad start point error"){
				REQUIRE(findMessage(testResult,"/startwrong/sample/transform/depends_on", "error",
				"Cannot even find the starting point of the depends_on chain"));
			}
    }
  }
}
SCENARIO("Testing for failure when depends_on attribute points nowhere ","[depnotfound]"){
  GIVEN("An initialized validation context"){
    pNXVcontext con = NXVinit("data");
    multimap<string,string> *testResult = prepareTest(con);
    REQUIRE(con != NULL);

    WHEN("Validating depends.h5"){
      int status = NXVvalidate(con,"data/depends.h5", NULL, NULL);
			THEN("Path /notfound/sample/transform/x should report element not found"){
				REQUIRE(findMessage(testResult,"/notfound/sample/transform/x", "error",
				"/good/sample/schnueffel pointing nowhere"));
			}
    }
  }
}
SCENARIO("Flag error on missing depends_on attribute == broken chain ","[depbroken]"){
  GIVEN("An initialized validation context"){
    pNXVcontext con = NXVinit("data");
    multimap<string,string> *testResult = prepareTest(con);
    REQUIRE(con != NULL);

    WHEN("Validating depends.h5"){
      int status = NXVvalidate(con,"data/depends.h5", NULL, NULL);
			THEN("Path /unterminated/sample/transform/z should report missing depends_on attribute"){
				REQUIRE(findMessage(testResult,"/unterminated/sample/transform/z", "error",
				"Missing attribute depends_on"));
			}
    }
  }
}
SCENARIO("Flag errors on depends_on family attributes ","[depbadatt]"){
  GIVEN("An initialized validation context"){
    pNXVcontext con = NXVinit("data");
    multimap<string,string> *testResult = prepareTest(con);
    REQUIRE(con != NULL);

    WHEN("Validating depends.h5"){
      int status = NXVvalidate(con,"data/depends.h5", NULL, NULL);
			THEN("Path /badatt/sample/transform/x should report bad data on transformation_type "){
				REQUIRE(findMessage(testResult,"/badatt/sample/transform/x", "error",
				"contains bad data"));
			}
			THEN("Path /badatt/sample/transform/x should report wrong type on vector "){
				REQUIRE(findMessage(testResult,"/badatt/sample/transform/x", "error",
				"vector attribute on /badatt/sample/transform/x is of wrong type"));
			}
			THEN("Path /badatt/sample/transform/z should report wrong type on offset "){
				REQUIRE(findMessage(testResult,"/badatt/sample/transform/z", "error",
				"offset attribute on /badatt/sample/transform/z is of wrong type"));
			}
			THEN("Path /badatt/sample/transform/z should report wrong size on vector "){
				REQUIRE(findMessage(testResult,"/badatt/sample/transform/z", "error",
				"vector attribute on /badatt/sample/transform/z is of wrong size"));
			}
    }
  }
}
