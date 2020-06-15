#include "CustomNodeGrouper.h"

#include "Global.h"
#include "Node.h"

// --------------------------------------------------------------------
//   findTopNodesX0
// --------------------------------------------------------------------

void CustomNodeGrouper::findTopNodesX0

  ( Global&   globdat,
    const int nn0 )

{
  int nn = globdat.nodeSet.size();
  double zinterf = 0.;

  NodePointer np;

  IntVector inx0;

  for ( int in = 0; in < nn; ++in )
  {
    np = globdat.nodeSet[in];
    int index = np->getIndex();

    if ( np->getX() < 1.e-6 )
    {
      inx0.push_back ( in );

      int duplicity = np->getDuplicity();

      if ( duplicity > 1 )
      {
        zinterf = np->getY();

        cout << "found interface at " << zinterf << endl;

        for ( int dupl = 1; dupl < duplicity; ++dupl )
        {
          int idup = globdat.duplicatedNodes[index][dupl];
          inx0.push_back ( idup );
        }
      }
    }
  }

  int nx0 = inx0.size();

  for ( int in = 0; in < nx0; ++in )
  {
    int inode = inx0[in];
    
    if ( inode < nn0 )
    {
      np = globdat.nodeSet[inode];

      if ( np->getY() > zinterf )
      {
        inodes_.push_back ( np->getIndex() );
      }
    }
    else
    {
      int id = inode; // use indes
      inodes_.push_back ( id );
    }
  }
}

// --------------------------------------------------------------------
//   writeNodeGroup
// --------------------------------------------------------------------

void CustomNodeGrouper::writeNodeGroup

  ( const char* fileName )

{
  cout << "Writing nodegroup " << inodes_.size() << "\n";
  print(inodes_.begin(), inodes_.end(), " ");
  cout << endl;

  string filename ( fileName );

  ofstream file ( fileName, std::ios::out );

  file << "<NodeGroup name=\"leftsym\">\n{" << inodes_[0];

  for ( int in = 1; in < inodes_.size(); ++in )
  {
    file << ", " << inodes_[in];
  }
  file << "}\n</NodeGroup>\n";

  file.close();
}
