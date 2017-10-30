/**
  * This file contains tests for testing dataset number types
	* and dimension validation code of nxvalidate
	*
	* copyright: GPL
	*
	* Mark Koennecke, mark.koennecke@psi.ch, and NIAC
	* January 2016
	*/
#include "catch.hpp"
#include <nxvalidate.h>
#include "testutil.hpp"

SCENARIO("Testing for dimension match","[dim]"){
  GIVEN("An initialized validation context"){
    pNXVcontext con = NXVinit("data");
    multimap<string,string> *testResult = prepareTest(con);
    REQUIRE(con != NULL);

    WHEN("Validating dsunidim.h5"){
      int status = NXVvalidate(con,"data/dsunidim.h5", NULL, NULL);

      THEN("Path /entry/gooddata should not be in error"){
        REQUIRE(testNoError(testResult,"/entry/gooddata"));
      }
			THEN("Path /entry/mismatchdim should report error Dimension mismatch"){
				REQUIRE(findMessage(testResult,"/entry/mismatchdim", "error",
				"Dimension mismatch on"));
			}
			THEN("Path /entry/mismatchrankshould report error Rank mismatch"){
				REQUIRE(findMessage(testResult,"/entry/mismatchrank", "error",
				"Rank mismatch"));
			}
    }
  }
}

SCENARIO("Testing for rank as a symbol","[rankSym]"){
  GIVEN("An initialized validation context"){
    pNXVcontext con = NXVinit("data");
    multimap<string,string> *testResult = prepareTest(con);
    REQUIRE(con != NULL);

    WHEN("Validating dsunidim.h5"){
      int status = NXVvalidate(con,"data/dsunidim.h5", NULL, NULL);

      THEN("Path /entry/ranksymbol should not be in error"){
        REQUIRE(testNoError(testResult,"/entry/ranksymbol"));
      }
    }
  }
}


SCENARIO("Testing the use of dimension symbols","[dimsym]"){
  GIVEN("An initialized validation context"){
    pNXVcontext con = NXVinit("data");
    multimap<string,string> *testResult = prepareTest(con);
    REQUIRE(con != NULL);

    WHEN("Validating dsunidim.h5"){
      int status = NXVvalidate(con,"data/dsunidim.h5", NULL, NULL);

      THEN("Path /entry/symbolok should not be in error"){
        REQUIRE(testNoError(testResult,"/entry/symbolok"));
      }
			THEN("Path /entry/symbolbad should report error Dimension mismatch"){
				REQUIRE(findMessage(testResult,"/entry/symbolbad", "error",
				"Dimension mismatch on"));
			}
    }
  }
}

SCENARIO("Testing NX_INT type checking ","[nxint]"){
  GIVEN("An initialized validation context"){
    pNXVcontext con = NXVinit("data");
    multimap<string,string> *testResult = prepareTest(con);
    REQUIRE(con != NULL);

    WHEN("Validating dsunidim.h5"){
      int status = NXVvalidate(con,"data/dsunidim.h5", NULL, NULL);

      THEN("Path /entry/goodint should not be in error"){
        REQUIRE(testNoError(testResult,"/entry/goodint"));
      }
			THEN("Path /entry/badint should report error Data Type mismatch"){
				REQUIRE(findMessage(testResult,"/entry/badint", "error",
				"Data type mismatch"));
			}
    }
  }
}

SCENARIO("Testing NX_POSINT type checking ","[nxposint]"){
  GIVEN("An initialized validation context"){
    pNXVcontext con = NXVinit("data");
    multimap<string,string> *testResult = prepareTest(con);
    REQUIRE(con != NULL);

    WHEN("Validating dsunidim.h5"){
      int status = NXVvalidate(con,"data/dsunidim.h5", NULL, NULL);

      THEN("Path /entry/goodposint should not be in error"){
        REQUIRE(testNoError(testResult,"/entry/goodposint"));
      }
			THEN("Path /entry/badposint should report error Data Type mismatch"){
				REQUIRE(findMessage(testResult,"/entry/badposint", "error",
				"Data type mismatch"));
			}
    }
  }
}

SCENARIO("Testing NX_UINT type checking ","[nxuint]"){
  GIVEN("An initialized validation context"){
    pNXVcontext con = NXVinit("data");
    multimap<string,string> *testResult = prepareTest(con);
    REQUIRE(con != NULL);

    WHEN("Validating dsunidim.h5"){
      int status = NXVvalidate(con,"data/dsunidim.h5", NULL, NULL);

      THEN("Path /entry/gooduint should not be in error"){
        REQUIRE(testNoError(testResult,"/entry/gooduint"));
      }
			THEN("Path /entry/baduint should report error Data Type mismatch"){
				REQUIRE(findMessage(testResult,"/entry/baduint", "error",
				"Data type mismatch"));
			}
			THEN("Path /entry/worseuint should report error Data Type mismatch"){
				REQUIRE(findMessage(testResult,"/entry/worseuint", "error",
				"Data type mismatch"));
			}
    }
  }
}

SCENARIO("Testing NX_FLOAT type checking ","[nxfloat]"){
  GIVEN("An initialized validation context"){
    pNXVcontext con = NXVinit("data");
    multimap<string,string> *testResult = prepareTest(con);
    REQUIRE(con != NULL);

    WHEN("Validating dsunidim.h5"){
      int status = NXVvalidate(con,"data/dsunidim.h5", NULL, NULL);

      THEN("Path /entry/goodfloat should not be in error"){
        REQUIRE(testNoError(testResult,"/entry/goodfloat"));
      }
			THEN("Path /entry/badfloat should report error Data Type mismatch"){
				REQUIRE(findMessage(testResult,"/entry/badfloat", "error",
				"Data type mismatch"));
			}
    }
  }
}

SCENARIO("Testing NX_NUMBER type checking ","[nxnum]"){
  GIVEN("An initialized validation context"){
    pNXVcontext con = NXVinit("data");
    multimap<string,string> *testResult = prepareTest(con);
    REQUIRE(con != NULL);

    WHEN("Validating dsunidim.h5"){
      int status = NXVvalidate(con,"data/dsunidim.h5", NULL, NULL);

      THEN("Path /entry/goodnumber should not be in error"){
        REQUIRE(testNoError(testResult,"/entry/goodnumber"));
      }
			THEN("Path /entry/alsogoodnumber should not be in error"){
        REQUIRE(testNoError(testResult,"/entry/alsogoodnumber"));
      }
			THEN("Path /entry/badnumber should report error Data Type mismatch"){
				REQUIRE(findMessage(testResult,"/entry/badnumber", "error",
				"Data type mismatch"));
			}
    }
  }
}

SCENARIO("Testing NX_CHAR type checking ","[nxchar]"){
  GIVEN("An initialized validation context"){
    pNXVcontext con = NXVinit("data");
    multimap<string,string> *testResult = prepareTest(con);
    REQUIRE(con != NULL);

    WHEN("Validating dsunidim.h5"){
      int status = NXVvalidate(con,"data/dsunidim.h5", NULL, NULL);

      THEN("Path /entry/goodtxt should not be in error"){
        REQUIRE(testNoError(testResult,"/entry/goodtxt"));
      }
			THEN("Path /entry/badtxt should report error Data Type mismatch"){
				REQUIRE(findMessage(testResult,"/entry/badtxt", "error",
				"Data type mismatch"));
			}
    }
  }
}

SCENARIO("Testing NX_DATE_TIME type checking ","[nxdate]"){
  GIVEN("An initialized validation context"){
    pNXVcontext con = NXVinit("data");
    multimap<string,string> *testResult = prepareTest(con);
    REQUIRE(con != NULL);

    WHEN("Validating dsunidim.h5"){
      int status = NXVvalidate(con,"data/dsunidim.h5", NULL, NULL);

      THEN("Path /entry/goodtime with T format variant should not be in error"){
        REQUIRE(testNoError(testResult,"/entry/goodtime"));
      }
			THEN("Path /entry/moregoodtime with space format variant should not be in error"){
        REQUIRE(testNoError(testResult,"/entry/moregoodtime"));
      }
			THEN("Path /entry/badtime should report error Data Type mismatch"){
				REQUIRE(findMessage(testResult,"/entry/badtime", "error",
				"Data type mismatch"));
			}
			THEN("Path /entry/badstringtime should report error not in ISO8601 format"){
				REQUIRE(findMessage(testResult,"/entry/badstringtime", "error",
				"Date/Time not in ISO8601 format"));
			}
    }
  }
}


SCENARIO("Testing NX_BINARY type checking ","[nxbin]"){
  GIVEN("An initialized validation context"){
    pNXVcontext con = NXVinit("data");
    multimap<string,string> *testResult = prepareTest(con);
    REQUIRE(con != NULL);

    WHEN("Validating dsunidim.h5"){
      int status = NXVvalidate(con,"data/dsunidim.h5", NULL, NULL);

      THEN("Path /entry/goodbin should not be in error"){
        REQUIRE(testNoError(testResult,"/entry/goodbin"));
      }
			THEN("Path /entry/badbin should report error Data Type mismatch for size"){
				REQUIRE(findMessage(testResult,"/entry/badbin", "error",
				"Data type mismatch"));
			}
			THEN("Path /entry/reallybadbin should report error Data Type mismatch for type"){
				REQUIRE(findMessage(testResult,"/entry/reallybadbin", "error",
				"Data type mismatch"));
			}
    }
  }
}

SCENARIO("Testing NX_BOOLEAN type checking ","[nxbool]"){
  GIVEN("An initialized validation context"){
    pNXVcontext con = NXVinit("data");
    multimap<string,string> *testResult = prepareTest(con);
    REQUIRE(con != NULL);

    WHEN("Validating dsunidim.h5"){
      int status = NXVvalidate(con,"data/dsunidim.h5", NULL, NULL);

      THEN("Path /entry/goodbool should not be in error"){
        REQUIRE(testNoError(testResult,"/entry/goodbool"));
      }
			THEN("Path /entry/badbool should report error Data Type mismatch for size"){
				REQUIRE(findMessage(testResult,"/entry/badbool", "error",
				"Data type mismatch"));
			}
			THEN("Path /entry/reallybadbool should report error Data Type mismatch for type"){
				REQUIRE(findMessage(testResult,"/entry/reallybadbool", "error",
				"Data type mismatch"));
			}
    }
  }
}
