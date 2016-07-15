/*
 Just a morsel of code to define the minimal application definition. For 
 checking NeXus files without an application definition
*/

static const char xmlMinimal[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
<?xml-stylesheet type=\"text/xsl\" href=\"nxdlformat.xsl\" ?>\
<definition name=\"NXminimal\" extends=\"NXobject\" type=\"group\"\
  category=\"application\"\
  xmlns=\"http://definition.nexusformat.org/nxdl/3.1\"\
    xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\
    xsi:schemaLocation=\"http://definition.nexusformat.org/nxdl/3.1 ../nxdl.xsd\"\
    version=\"1.0b\"\
    >\
    <group type=\"NXentry\" name=\"entry\">\
   </group>\
</definition>";
