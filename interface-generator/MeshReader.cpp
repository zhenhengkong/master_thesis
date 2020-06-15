
#include <boost/algorithm/string.hpp>


#include "MeshReader.h"
#include "Global.h"

// =====================================================================
//     readMesh
// =====================================================================


void                     readMesh 

   ( Global&     globdat,
     const char* fileName )

{
  string    filename  ( fileName );
  StrVector filenames;

  boost::split ( filenames, filename, boost::is_any_of(".") );

  if      ( filenames[1] == "msh" )
  {
    readGmshMesh ( globdat, fileName );
  }
  else if ( filenames[1] == "nurbs" )
  {
    readNURBSMesh ( globdat, fileName );
    globdat.isNURBS = true;
  }
  else
  {
    cout << "not yet supported!!!\n";
    exit(1);
  }

  
  // summarise the mesh a bit

  cout << "MESH SUMMARY:\n";

  cout << "Number of nodes............................... " << globdat.nodeSet.size       () << endl;
  cout << "Number of boundary nodes...................... " << globdat.boundaryNodes.size () << endl;
  cout << "Number of elements............................ " << globdat.elemSet.size       () << endl;
  cout << "Number of element groups...................... " << globdat.dom2Elems.size     () << endl;

  cout << endl;
}


