/**
  * This file contains tests for the basic operation of cnxvalidate.
	* This is stuff like finding files, recursion etc.
	*
	* copyright: GPL
	*
	* Mark Koennecke, mark.koennecke@psi.ch, and NIAC
	* December 2015
	*/
#include "catch.hpp"
#include <nxvalidate.h>
#include "testutil.hpp"

SCENARIO("Failure when no file","[nofile]"){
  GIVEN("An initialized validation context"){
    pNXVcontext con = NXVinit("data");
    multimap<string,string> *testResult = prepareTest(con);
    REQUIRE(con != NULL);

    WHEN("Validating doesnotexist.h5"){
      int status = NXVvalidate(con,"doesnotexist.h5", NULL, NULL);

      THEN("Path / should report fatal error file not found"){
        REQUIRE(findMessage(testResult,"/", "fatal","Failed to open"));
      }
    }
  }
}

SCENARIO("Missing file attributes","[noatt]"){
  GIVEN("An initialized validation context"){
    pNXVcontext con = NXVinit("data");
    multimap<string,string> *testResult = prepareTest(con);
    REQUIRE(con != NULL);

    WHEN("Validating nofileattributes.h5"){
      int status = NXVvalidate(con,"data/nofileattributes.h5", NULL, NULL);

      THEN("Path / should report error Missing global attribute file_name"){
        REQUIRE(findMessage(testResult,"/","error",
        "Missing required global file_name"));
      }
      AND_THEN("Path / should report error Missing global attribute file_name"){
        REQUIRE(findMessage(testResult,"/","error",
        "Missing required global file_time"));
      }
    }
  }
}

SCENARIO("File attributes OK","[okatt]"){
  GIVEN("An initialized validation context"){
    pNXVcontext con = NXVinit("data");
    multimap<string,string> *testResult = prepareTest(con);
    REQUIRE(con != NULL);

    WHEN("Validating fileattributes.h5"){
      int status = NXVvalidate(con,"data/fileattributes.h5", NULL, NULL);

      THEN("Path / should report no error Missing global attribute file_name"){
        REQUIRE(!findMessage(testResult,"/","error",
        "Missing required global file_name"));
      }
      AND_THEN("Path / should report no error Missing global attribute file_name"){
        REQUIRE(!findMessage(testResult,"/","error",
        "Missing required global file_time"));
      }
    }
  }
}

SCENARIO("Malformed file time attribute","[badfiletime]"){
  GIVEN("An initialized validation context"){
    pNXVcontext con = NXVinit("data");
    multimap<string,string> *testResult = prepareTest(con);
    REQUIRE(con != NULL);

    WHEN("Validating badtimeattributes.h5"){
      int status = NXVvalidate(con,"data/badtimeattributes.h5", NULL, NULL);

      THEN("Path / should report error file_time not in ISO8601"){
        REQUIRE(findMessage(testResult,"/","error",
        "file_time is not in ISO 8601"));
      }
    }
  }
}

SCENARIO("Missing application definition","[missappdef]"){
  GIVEN("An initialized validation context"){
    pNXVcontext con = NXVinit("data");
    multimap<string,string> *testResult = prepareTest(con);
    REQUIRE(con != NULL);

    WHEN("Validating badtimeattributes.h5"){
      int status = NXVvalidate(con,"data/badtimeattributes.h5", NULL, NULL);

      THEN("Path / should report error failed to find application definition"){
        REQUIRE(findMessage(testResult,"/entry","error",
        "failed to find application definition"));
      }
    }
  }
}
