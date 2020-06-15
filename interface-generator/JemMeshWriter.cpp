#include "Global.h"
#include "Node.h"
#include "Element.h"


#include <boost/algorithm/string.hpp>


// =====================================================================
//     writeJemMesh
// =====================================================================

void                     writeJemMesh 

    ( Global&     globdat,
      const char* fileName )
{
  ofstream file ( fileName, std::ios::out );
  
  cout << "Writing nodes...\n";

  file << "<Nodes>\n";

  const int nodeCount = globdat.newNodeSet.size ();
  const int elemCount = globdat.elemSet.size    ();

  NodePointer np;

  for ( int in = 0; in < nodeCount; in ++ )
  {
    np = globdat.newNodeSet[in];
 
    if ( !globdat.is3D )
    {
      file << np->getIndex() 
           << " " << np->getX() << " " << np->getY() << ";\n";
    }
    else
    {
      file << np->getIndex() << " " 
           << np->getX() << " " 
           << np->getY() << " "
           << np->getZ() << ";\n";
    }
  }

  file << "</Nodes>\n";

  cout << "Writing nodes...done!\n" << endl;

  cout << "Writing elements..." << endl;

  file << "\n<Elements>\n";

  ElemPointer ep;
  IntVector   connect, newConnect;

  for ( int ie = 0; ie < elemCount; ie ++ )
  {
    ep = globdat.elemSet[ie];

    file << ep->getIndex () << " ";

    ep->getJemConnectivity ( connect ); 

    copy ( connect.begin(), connect.end(), ostream_iterator<int> (file, " " ) );

    file << ";\n";
  }


  file << "</Elements>\n";

  cout << "Writing elements...done!\n\n";

  cout << "Writing element groups...\n";

  const int domCount = globdat.dom2Elems.size ();

  if ( domCount != 1 )
  {
    Int2IntVectMap::iterator it  = globdat.dom2Elems.begin ();
    Int2IntVectMap::iterator eit = globdat.dom2Elems.end   ();

    for ( ; it != eit; ++it )
    {
      int       domName = it->first; 
      IntVector elems   = it->second; 

      file << "<ElementGroup name=\"solid" << domName << "\">\n{";
      
      for ( int ie = 0; ie < elems.size()-1; ie++ )
      {
        file << elems[ie] << ",";
      }

      file << elems[elems.size()-1] << "}\n"
           << "</ElementGroup>\n\n";
    }
  }

  cout << "Writing element groups...done!\n\n";

  cout << "Writing node groups...\n";

  const int nodeGrpCount = globdat.bndNodesMap.size ();

  if ( nodeGrpCount != 1 )
  {
    Int2IntSetMap::iterator it  = globdat.bndNodesMap.begin ();
    Int2IntSetMap::iterator eit = globdat.bndNodesMap.end   ();

    for ( ; it != eit; ++it )
    {
      int       domName = it->first; 
      IntSet    elems   = it->second; 

      IntVector   nodes;
      copy ( elems.begin(), elems.end(), back_inserter(nodes) );

      file << "<NodeGroup name=\"" << domName << "\">\n{";
      
      for ( int ie = 0; ie < nodes.size()-1; ie++ )
      {
        file << nodes[ie] << ",";
      }

      file << nodes[nodes.size()-1] << "}\n"
           << "</NodeGroup>\n\n";
    }
  }

  const int isoNodeCount = globdat.isolatedNodes.size ();

  if ( isoNodeCount != 0 )
  {
    for ( int in = 0; in < isoNodeCount; ++in )
    {

      file << "<NodeGroup name=\"" << 999 << "\">\n{"
           << globdat.isolatedNodes[in] << "}\n"
           << "</NodeGroup>\n\n";
    }
  }

  cout << "Writing node groups...done!\n\n";
}

