Description of the Inner Workings of cnxvalidate
==================================================

Cnxvalidate's main purpose is to validate NeXus HDF5 files against
an application definition. Along the way, it also tests certain
NeXus features. The general approach is to validate as much as
possible and keep running even in the face of errors.

# Validation at the File Level

Cnxvalidate will test for the existence of the NeXus global file attributes
file_name and file_time. It also checks if file_time contains a valid
ISO8601 data time string.

# Bootstrapping the Validation

For validation, cnxvalidate needs a NXentry or a NXsubentry  and an
application definition as input. This is about how cnxvalidate finds its
inputs. There are several cases:

The user has specified both  a path into the file and an application definition.
This overrides everything and cnxvalidate will validate the specified path
against the specified application definition.

The user has specified an application definition. Then cnxvalidate will search
all NXentry at root level and valöidate them against the application definition.

The user has specified nothing and cnxvalidate has to find its way round the
file itself. The following pseudocode is run:

    For each root level group in file:
		   If the group is of type NXentry:
			    Try to find the definition field
					If the definition field is present:
					   Validate the NXentry against the application definition specified
						  in the definition field
					else:
					    Foreach NXsubentry group in the NXentry:
							   Find the definition field:
								 If the definition field is present:
								    validate the NXsubentry against the application definition
										specified in the definition field
							When no NXsubentry to validate could be found:
							   Give up


# The Actual Validation

There is some initial clearing of contexts and setting up to do.
Most importantly the application definition is searched for and loaded.
When the application definition uses inheritance, the base application
definition is loaded too and merged into the NXDL tree of the top level
application definition. This is done in such a way that top level specifications
shadow base class specifications.

When this is done the actual validation is started by validating the entry
level group.

# Group Validation

The group validation code drives the actual validation of the file by recursing
into deeper level groups.

Group validation is actually a two pass process. Before the two passes, group
attributes are validated. As of now this only checks for the NX_class group
attribute.

In the first pass, all NXDL elements are inspected. For each element there is a
test if the equivalent element exists at that level in the HDF5 file. If not,
appropriate errors and warning are flagged.  Further processing depends on the
type of the element:

* For fields and links the appropriate validation functions are called
* For a depends_on field, the depends_on chain validation is called
* For groups, the context is advanced and validateGroup is called for
  driving the recursion.

Each element found in the first pass is entered into a list, called namesSeen.

Before the second pass we load the base class definition of the NeXus group
we are in.  In the second pass, we iterate over all elements of the HDF5 group we are
validating. First we test if we have already seen the element in the first pass
and if so, skip over it.

If we have not seen it,  we  check if the name occurs in the base class and if
so warn about an additional base class element. If the element is not in the
base class, cnxvalidate warns about an additional non-NeXus field.

# Field Validation

This consists of attribute validation, dimension and data validation.

When specified, the rank and the dimensions in the NXDL file are compared with
the dimensions of the HDF5 field and any mismatches will be reported. NXDL can
contain symbols for dimension fields. For this case cnxvalidate maintains a
symbol dictionary. The first occurrence of the symbol defines the value. Later
occurrences are validated against the stored value in the dictionary.

Some times data is specified in the NXDL file as an enumeration. This is also
validated. This validation only works for text fields.

## Dataset Attribute Validation

Theres is some complexity here. The first one is that attributes can either
be specified as XML attributes to the field NXDL element or as separate
attribute tags underneath the field tag. Both must be validated. Then, each
attribute needs its own validation code. The CIF attributes, vector, offset,
transformation_type and depends_on must be exempted from attribute validation
here as they are validated as part of the depends_on chain validation. In
addition we can do some NeXus validation on attributes which may not be
specified in the application definition. The current implementation is
somewhat informed by the current usage of attributes in application definitions.
Cnxvalidate contains a dictionary which maps an attribute name to a validation
function.

     The type attribute is searched and validated
		 The units attribute is searched for and validated. This validation is
			    very limited
   	 Foreach field element attribute in NXDL:
		    Check for presence in the HDF5 file
				When present:
				   Retrieve its validation function from the dictionary and
					 invoke it
				else:
				   Flag an error
		 Foreach attribute child of the field element:
		    Check for presence in the HDF5 file
				When present:
				   Retrieve the validation function from the dictionary and invoke it
				else:
				   Flag an error
		 Validate data_offset and stride for consistency when present in the HDF5 file
		 Validate the interpretation attribute when present in the HDF5 file			 		 
		 Validate the calibration attribute for consistency when present in the
		   HDF5 file

This may need a refactoring.

# Depends_on Chain Validation

Basically this routine tries to follow the depends_on chain and checks all
datasets involved. It flags an error when the depends_on chain is broken.

For each dataset in the depends_on chain the presence of the CIF attributes
is validated. As are appropriate types and ranks for the offset and vector
attributes. The transformation_type attribute is also checked for type and
that it contains either translation or rotation.

# Link Validation

This tests if the link exists in the HDF5 file and points to a valid HDF5
object. A further test looks for the target attribute and complains when the
attribute is missing. The target attribute is read and its value is tested
in two ways:

* The target must point to a valid HDF5 object
* The group hierarchy must math the specification in the NXDL file

  
