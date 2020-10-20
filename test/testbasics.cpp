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
      int status = NXVvalidate(con,"doesnotexist.h5", NULL, NULL, 0);

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
      int status = NXVvalidate(con,"data/nofileattributes.h5", NULL, NULL, 1);

      THEN("Path / should report warning Missing recommende attribute file_name"){
        REQUIRE(findMessage(testResult,"/","warnopt",
        "Missing recommended global file_name"));
      }
      AND_THEN("Path / should report warning Missing recommended attribute file_name"){
        REQUIRE(findMessage(testResult,"/","warnopt",
        "Missing recommended global file_time"));
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
      int status = NXVvalidate(con,"data/fileattributes.h5", NULL, NULL, 0);

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
      int status = NXVvalidate(con,"data/badtimeattributes.h5", NULL, NULL, 0);

      THEN("Path / should report error file_time not in ISO8601"){
        REQUIRE(findMessage(testResult,"/","error",
        "file_time is not in ISO 8601"));
      }
    }
  }
}

SCENARIO("Missing application definition in entry","[missappdef]"){
  GIVEN("An initialized validation context"){
    pNXVcontext con = NXVinit("data");
    multimap<string,string> *testResult = prepareTest(con);
    REQUIRE(con != NULL);

    WHEN("Validating badtimeattributes.h5"){
      int status = NXVvalidate(con,"data/badtimeattributes.h5", NULL, NULL, 0);

      THEN("Path /entry should report error failed to find application definition"){
        REQUIRE(findMessage(testResult,"/entry","error",
        "failed to find application definition"));
      }
    }
  }
}

SCENARIO("Locate an application definition from a definition field", "[locdef]"){
  GIVEN("An initialized validation context"){
    pNXVcontext con = NXVinit("data");
    multimap<string,string> *testResult = prepareTest(con);
    REQUIRE(con != NULL);

    WHEN("Validating manyentries.hy"){
      int status = NXVvalidate(con,"data/manyentries.h5", NULL, NULL, 0);

      THEN("Path /entry0 should validate against NXminimal"){
        REQUIRE(findMessage(testResult, "/entry0","debug",
          "Validating /entry0 against NXminimal"));
      }
    }
  }
}

SCENARIO("Validating multiple entries","[multientry]"){
  GIVEN("An initialized validation context"){
    pNXVcontext con = NXVinit("data");
    multimap<string,string> *testResult = prepareTest(con);
    REQUIRE(con != NULL);

    WHEN("Validating manyentries.hy"){
      int status = NXVvalidate(con,"data/manyentries.h5", NULL, NULL, 0);

      THEN("Path /entry0 should validate against NXminimal"){
        REQUIRE(findMessage(testResult, "/entry0","debug",
          "Validating /entry0 against NXminimal"));
      }
      THEN("Path /entry1 should validate against NXminimal"){
        REQUIRE(findMessage(testResult, "/entry1","debug",
          "Validating /entry1 against NXminimal"));
      }
      THEN("Path /entry2 should validate against NXminimal"){
        REQUIRE(findMessage(testResult, "/entry2","debug",
          "Validating /entry2 against NXminimal"));
      }
    }
  }
}

SCENARIO("Complain about validating against a non existent application definition",
  "[badappdef]"){
    GIVEN("An initialized validation context"){
      pNXVcontext con = NXVinit("data");
      multimap<string,string> *testResult = prepareTest(con);
      REQUIRE(con != NULL);

      WHEN("Validating shittyapp.hy"){
        int status = NXVvalidate(con,"data/shittyapp.h5", NULL, NULL, 0);

        THEN("Path /entry should validate against NXshitty"){
          REQUIRE(findMessage(testResult, "/entry","debug",
            "Validating /entry against NXshitty"));
        }
        THEN("Path /entry should report error failed to load application definition"){
          REQUIRE(findMessage(testResult,"/entry","fatal",
          "Failed to load application definition"));
        }
      }
  }
}

SCENARIO("Validate against a specified application definition",
  "[specdef]") {

    GIVEN("An initialized validation context"){
      pNXVcontext con = NXVinit("data");
      multimap<string,string> *testResult = prepareTest(con);
      REQUIRE(con != NULL);

      WHEN("Validating shittyapp.h5 against NXminimal"){
        int status = NXVvalidate(con,"data/shittyapp.h5", "NXminimal", NULL, 0);

        THEN("Path /entry should validate against NXminimal"){
          REQUIRE(findMessage(testResult, "/entry","debug",
            "Validating /entry against NXminimal"));
        }
      }
    }
}

SCENARIO("Validating against an application defintion with inheritance",
"[appinherit]"){

  GIVEN("An initialized validation context"){
    pNXVcontext con = NXVinit("data");
    multimap<string,string> *testResult = prepareTest(con);
    REQUIRE(con != NULL);

    WHEN("Validating inherit.h5"){
      int status = NXVvalidate(con,"data/inherit.h5", NULL, NULL, 0);

      THEN("Field /entry/title should validate"){
        REQUIRE(findMessage(testResult, "/entry/title","debug",
          "Validating field"));
      }
      THEN("Field /entry/instrument should validate"){
        REQUIRE(findMessage(testResult, "/entry/instrument","debug",
          "Validating field"));
      }
    }
  }
}

SCENARIO("Validating against multiple subentries","[multisub]"){
  GIVEN("An initialized validation context"){
    pNXVcontext con = NXVinit("data");
    multimap<string,string> *testResult = prepareTest(con);
    REQUIRE(con != NULL);

    WHEN("Validating subentry.h5"){
      int status = NXVvalidate(con,"data/subentry.h5", NULL, NULL, 0);

      THEN("Path /entry/sub1 should validate against NXminimal"){
        REQUIRE(findMessage(testResult, "/entry/sub1","debug",
          "Validating /entry/sub1 against NXminimal"));
      }
      THEN("Path /entry/sub2 should validate against NXminimal"){
        REQUIRE(findMessage(testResult, "/entry/sub2","debug",
          "Validating /entry/sub2 against NXminimal"));
      }
      THEN("Path /entry/sub3 should validate against NXminimal"){
        REQUIRE(findMessage(testResult, "/entry/sub3","debug",
          "Validating /entry/sub3 against NXminimal"));
      }
    }
  }
}
/*
 This is about NXsubentry being validated against an NXDL which
 usually starts on NXentry. I suppress this non-error
*/
SCENARIO("Omit wrong group error when checking NXsubentry","[subentry]"){
  GIVEN("An initialized validation context"){
    pNXVcontext con = NXVinit("data");
    multimap<string,string> *testResult = prepareTest(con);
    REQUIRE(con != NULL);

    WHEN("Validating subentry.h5"){
      int status = NXVvalidate(con,"data/subentry.h5", NULL, NULL, 0);

      THEN("Path /entry/sub1 should not give a wrong group error"){
        REQUIRE_FALSE(findMessage(testResult, "/entry/sub1","error",
          "Wrong group type"));
      }
    }
  }
}

SCENARIO("Fail to validate NXsubentry with missing definition","[subNO]"){
  GIVEN("An initialized validation context"){
    pNXVcontext con = NXVinit("data");
    multimap<string,string> *testResult = prepareTest(con);
    REQUIRE(con != NULL);

    WHEN("Validating subNOdef.h5"){
      int status = NXVvalidate(con,"data/subNOdef.h5", NULL, NULL, 0);

      THEN("Path /entry/sub1 should report error failed to find application definition"){
        REQUIRE(findMessage(testResult,"/entry/sub1","error",
        "no application definition"));
      }
    }
  }
}
SCENARIO("Validate with path and application definition specified","[spec]"){
  GIVEN("An initialized validation context"){
    pNXVcontext con = NXVinit("data");
    multimap<string,string> *testResult = prepareTest(con);
    REQUIRE(con != NULL);

    WHEN("Validating subNOdef.h5 path /entry/sub1, application definition NXminimal"){
      int status = NXVvalidate(con,"data/subNOdef.h5",
        "NXminimal", "/entry/sub1", 0);

      THEN("Path /entry/sub1 should validate against NXminimal"){
        REQUIRE(findMessage(testResult, "/entry/sub1","debug",
          "Validating /entry/sub1 against NXminimal"));
      }
    }
  }
}
SCENARIO("Testing recursion through file","[recu]"){
  GIVEN("An initialized validation context"){
    pNXVcontext con = NXVinit("data");
    multimap<string,string> *testResult = prepareTest(con);
    REQUIRE(con != NULL);

    WHEN("Validating recurse.h5"){
      int status = NXVvalidate(con,"data/recurse.h5",
        NULL, NULL, 0);

        THEN("Field /entry/title should validate"){
          REQUIRE(findMessage(testResult, "/entry/title","debug",
            "Validating field"));
        }
        THEN("Field /entry/step1/title2 should validate"){
          REQUIRE(findMessage(testResult, "/entry/step1/title2","debug",
            "Validating field"));
        }
        THEN("Field /entry/step1/step2/title3 should validate"){
          REQUIRE(findMessage(testResult, "/entry/step1/step2/title3","debug",
            "Validating field"));
        }
        THEN("Group /entry should validate"){
          REQUIRE(findMessage(testResult, "/entry","debug",
            "Validating group"));
        }
        THEN("Group /entry/step1 should validate"){
          REQUIRE(findMessage(testResult, "/entry/step1","debug",
            "Validating group"));
        }
        THEN("Group /entry/step1/step2 should validate"){
          REQUIRE(findMessage(testResult, "/entry/step1/step2","debug",
            "Validating group"));
        }
    }
  }
}

SCENARIO("Validate a file with a variable length NX_class attribute","[var]"){
  GIVEN("An initialized validation context"){
    pNXVcontext con = NXVinit("data");
    multimap<string,string> *testResult = prepareTest(con);
    REQUIRE(con != NULL);

    WHEN("Validating writer_2.1.hdf5 "){
      int status = NXVvalidate(con,"data/writer_2.1.hdf5",
        NULL, NULL, 0);

      THEN("Path /entry should have no No NX_class attribute error"){
        REQUIRE(!findMessage(testResult, "/entry","debug",
          "No NX_class attribute"));
      }
    }
  }
}

SCENARIO("Validate a file with a variable length NX_class attribute and a variable length definition field","[var2]"){
  GIVEN("An initialized validation context"){
    pNXVcontext con = NXVinit("data");
    multimap<string,string> *testResult = prepareTest(con);
    REQUIRE(con != NULL);

    WHEN("Validating varlen.h5 "){
      int status = NXVvalidate(con,"data/varlen.h5",
        NULL, NULL, 0);

      THEN("Path /entry should have no No NX_class attribute error"){
        REQUIRE(!findMessage(testResult, "/entry","debug",
          "No NX_class attribute"));
      }
      THEN("Path /entry should not report error failed to find application definition"){
        REQUIRE(!findMessage(testResult,"/entry","error",
        "failed to find application definition"));
      }
    }
  }
}
