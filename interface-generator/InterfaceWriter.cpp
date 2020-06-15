#include "InterfaceWriter.h"
#include "Global.h"
#include "Element.h"
#include "Node.h"

// ---------------------------------------------------------
//   writeInterface
// ---------------------------------------------------------

void  writeInterface 

(       Global&  globdat,
  const char*    fileName )

{
  const int   ieCount = globdat.interfaceSet.size ();

  ElemPointer ep;

  IntVector   connec;
  IntVector   dupNodes;

  ofstream    file ( fileName, std::ios::out );
  
  int nsolid = globdat.elemSet.size();

  cout << "Writing interface elements...\n";

  file << "<Elements>\n" ;

  for ( int ie = 0; ie < ieCount; ie++ )
  {
    ep = globdat.interfaceSet[ie];

    file << ep->getIndex()+nsolid << " ";

    ep->getConnectivity ( connec );

    copy ( connec.begin(), connec.end(), ostream_iterator<int> ( file, " " ) );

    file << ";\n";
  }

  file << "</Elements>\n\n";

  file << "<ElementGroup name=\"interface\">\n{";

  for ( int ie = 0; ie < ieCount-1; ++ie )
  {
    file << ie+nsolid << ",";
  }
  file << ieCount-1+nsolid << "}\n" << "</ElementGroup>\n\n";

  if ( globdat.is3D )
  {
    file << "OppositeVertices\n";

    for ( int ie = 0; ie < ieCount; ie++ )
    {
      file << globdat.oppositeVertices[ie] << "\n";
    }
  }

  cout << "Writing interface elements...done!\n\n";

  file.close ();
}

