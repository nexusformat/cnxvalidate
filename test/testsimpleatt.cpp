/**
  * This file contains tests for the simpler parts of the
	* dataset attribute validation in nxvalidate
	*
	* copyright: GPL
	*
	* Mark Koennecke, mark.koennecke@psi.ch, and NIAC
	* January 2015
	*/
#include "catch.hpp"
#include <nxvalidate.h>
#include "testutil.hpp"

SCENARIO("Testing the signal attribute","[sig]"){
  GIVEN("An initialized validation context"){
    pNXVcontext con = NXVinit("data");
    multimap<string,string> *testResult = prepareTest(con);
    REQUIRE(con != NULL);

    WHEN("Validating dssimpleatt.h5"){
      int status = NXVvalidate(con,"data/dssimpleatt.h5", NULL, NULL);

      THEN("Path /entry/intsignal with signal as int should not be in error"){
        REQUIRE(testNoError(testResult,"/entry/intsignal"));
      }
			THEN("Path /entry/txtsignal with signal as NX_CHAR should not be in error"){
        REQUIRE(testNoError(testResult,"/entry/txtsignal"));
      }
			THEN("Path /entry/badsignal should report type error"){
				REQUIRE(findMessage(testResult,"/entry/badsignal", "error",
				"wrong data type"));
			}
			THEN("Path /entry/wrongsignal should report a bad signal value"){
				REQUIRE(findMessage(testResult,"/entry/wrongsignal", "error",
				"Invalid value"));
			}
    }
  }
}

SCENARIO("Testing the primary attribute","[prim]"){
  GIVEN("An initialized validation context"){
    pNXVcontext con = NXVinit("data");
    multimap<string,string> *testResult = prepareTest(con);
    REQUIRE(con != NULL);

    WHEN("Validating dssimpleatt.h5"){
      int status = NXVvalidate(con,"data/dssimpleatt.h5", NULL, NULL);

      THEN("Path /entry/intprim with primary as int should not be in error"){
        REQUIRE(testNoError(testResult,"/entry/intprim"));
      }
			THEN("Path /entry/txtprim with primary as NX_CHAR should not be in error"){
        REQUIRE(testNoError(testResult,"/entry/txtprim"));
      }
			THEN("Path /entry/badprim should report type error"){
				REQUIRE(findMessage(testResult,"/entry/badsignal", "error",
				"wrong data type"));
			}
			THEN("Path /entry/wrongprim should report a bad primary value"){
				REQUIRE(findMessage(testResult,"/entry/wrongprim", "error",
				"Invalid value"));
			}
    }
  }
}

SCENARIO("Testing the axis attribute","[axis]"){
  GIVEN("An initialized validation context"){
    pNXVcontext con = NXVinit("data");
    multimap<string,string> *testResult = prepareTest(con);
    REQUIRE(con != NULL);

    WHEN("Validating dssimpleatt.h5"){
      int status = NXVvalidate(con,"data/dssimpleatt.h5", NULL, NULL);

      THEN("Path /entry/intaxis with axis as int should not be in error"){
        REQUIRE(testNoError(testResult,"/entry/intprim"));
      }
			THEN("Path /entry/txtaxis with axis as NX_CHAR should not be in error"){
        REQUIRE(testNoError(testResult,"/entry/txtaxis"));
      }
			THEN("Path /entry/badaxis should report type error"){
				REQUIRE(findMessage(testResult,"/entry/badaxis", "error",
				"wrong data type"));
			}
			THEN("Path /entry/wrongaxis should report a bad axis value"){
				REQUIRE(findMessage(testResult,"/entry/wrongaxis", "error",
				"Invalid value"));
			}
    }
  }
}

SCENARIO("Testing the axes attribute","[axes]"){
  GIVEN("An initialized validation context"){
    pNXVcontext con = NXVinit("data");
    multimap<string,string> *testResult = prepareTest(con);
    REQUIRE(con != NULL);

    WHEN("Validating dssimpleatt.h5"){
      int status = NXVvalidate(con,"data/dssimpleatt.h5", NULL, NULL);

      THEN("Path /entry/colonaxes with axes separated by colon should not be in error"){
        REQUIRE(testNoError(testResult,"/entry/colonaxes"));
      }
			THEN("Path /entry/commaaxes with axes separated by , should not be in error"){
        REQUIRE(testNoError(testResult,"/entry/commaaxes"));
      }
			THEN("Path /entry/brokenaxes should report type error"){
				REQUIRE(findMessage(testResult,"/entry/brokenaxes", "error",
				"Invalid data type"));
			}
			THEN("Path /entry/morebrokenaxes should report a bad axes value"){
				REQUIRE(findMessage(testResult,"/entry/morebrokenaxes", "error",
				"Invalid value"));
			}
			THEN("Path /entry/invalidaxes should report that the axis is missing"){
				REQUIRE(findMessage(testResult,"/entry/invalidaxes", "error",
				"not found"));
			}
    }
  }
}
SCENARIO("Testing the data_offset and stride attributes","[stride]"){
  GIVEN("An initialized validation context"){
    pNXVcontext con = NXVinit("data");
    multimap<string,string> *testResult = prepareTest(con);
    REQUIRE(con != NULL);

    WHEN("Validating dssimpleatt.h5"){
      int status = NXVvalidate(con,"data/dssimpleatt.h5", NULL, NULL);

      THEN("Path /entry/goodoffsetstride should not be in error"){
        REQUIRE(testNoError(testResult,"/entry/goodoffsetstride"));
      }
			THEN("Path /entry/baddimoffsetstride should report a dimension mismatch"){
				REQUIRE(findMessage(testResult,"/entry/baddimoffsetstride", "error",
				"wrong dimension"));
			}
			THEN("Path /entry/badtypeoffsetstride should report a wrong data type"){
				REQUIRE(findMessage(testResult,"/entry/badtypeoffsetstride", "error",
				"wrong type"));
			}
			THEN("Path /entry/onemissingoffsetstride should report that one of data_offset or stride is missing"){
				REQUIRE(findMessage(testResult,"/entry/onemissingoffsetstride", "error",
				"one of data_offset or stride missing"));
			}
    }
  }
}

SCENARIO("Testing the calibration attribute","[calatt]"){
  GIVEN("An initialized validation context"){
    pNXVcontext con = NXVinit("data");
    multimap<string,string> *testResult = prepareTest(con);
    REQUIRE(con != NULL);

    WHEN("Validating dssimpleatt.h5"){
      int status = NXVvalidate(con,"data/dssimpleatt.h5", NULL, NULL);

      THEN("Path /entry/calnominal should not be in error"){
        REQUIRE(testNoError(testResult,"/entry/calnominal"));
      }
			THEN("Path /entry/calnmes should not be in error"){
        REQUIRE(testNoError(testResult,"/entry/calnominal"));
      }
			THEN("Path /entry/calwrong should report a invalid value error"){
				REQUIRE(findMessage(testResult,"/entry/calwrong", "error",
				"Invalid value"));
			}
			THEN("Path /entry/calbadtype should report a wrong data type"){
				REQUIRE(findMessage(testResult,"/entry/calbadtype", "error",
				"Invalid data type"));
			}
    }
  }
}
SCENARIO("Testing the interpretation attribute","[interp]"){
  GIVEN("An initialized validation context"){
    pNXVcontext con = NXVinit("data");
    multimap<string,string> *testResult = prepareTest(con);
    REQUIRE(con != NULL);

    WHEN("Validating dssimpleatt.h5"){
      int status = NXVvalidate(con,"data/dssimpleatt.h5", NULL, NULL);

      THEN("Path /entry/intscaler should not be in error"){
        REQUIRE(testNoError(testResult,"/entry/intscaler"));
      }
			THEN("Path /entry/intspec should not be in error"){
        REQUIRE(testNoError(testResult,"/entry/intspec"));
      }
			THEN("Path /entry/intimage should not be in error"){
        REQUIRE(testNoError(testResult,"/entry/intimage"));
      }
			THEN("Path /entry/intrgba should not be in error"){
        REQUIRE(testNoError(testResult,"/entry/intrgba"));
      }
			THEN("Path /entry/inthsla should not be in error"){
        REQUIRE(testNoError(testResult,"/entry/inthsla"));
      }
			THEN("Path /entry/intcmyk should not be in error"){
        REQUIRE(testNoError(testResult,"/entry/intcmyk"));
      }
      THEN("Path /entry/intvertex should not be in error"){
        REQUIRE(testNoError(testResult,"/entry/intvertex"));
      }
			THEN("Path /entry/calbadshould report a invalid value error"){
				REQUIRE(findMessage(testResult,"/entry/intbad", "error",
				"Invalid value"));
			}
			THEN("Path /entry/intadtype should report a wrong data type"){
				REQUIRE(findMessage(testResult,"/entry/intbadtype", "error",
				"Invalid data type"));
			}
    }
  }
}
