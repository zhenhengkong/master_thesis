#include <boost/algorithm/minmax_element.hpp>

#include "Element.h"
#include "Node.h"
#include "Global.h"


typedef vector<double>::const_iterator   iter;


// ------------------------------------------------------------
//    constructor
// ------------------------------------------------------------

Element::Element ( int index, int elemType,
                   const IntVector& connec )

  : index_(index), elemType_(elemType),
    connectivity_(connec), connectivity0_(connec), 
    done_(false), isQuadratic_(connec.size() > 4 ? true : false),
    isChanged_(false), isNURBS_(false)
{
  nodePerFace_ = 3;
}

Element::Element ( int index, int elemType,
                   const IntVector& connec, bool isNURBS )

  : index_(index), elemType_(elemType),
    connectivity_(connec), connectivity0_(connec), 
    done_(false), isQuadratic_(connec.size() > 4 ? true : false),
    isChanged_(false), isNURBS_(isNURBS)
{
  nodePerFace_ = 3;
}

Element::Element ( int index,
                   const IntVector& connec )

  : index_(index), 
    connectivity_(connec), connectivity0_(connec)
{
}

// ------------------------------------------------------------
//    buildFaces
// ------------------------------------------------------------
//
// attention: the faces must be oriented properly
// so that the normals are outward.

void Element::buildFaces ()
{
  if       ( elemType_ == 4 )   // 4 node tetrahedron
  {
    buildFacesForTet4_ ();
  }
  else if  ( elemType_ == 11 )  // 10 node tetrahedron
  {
    buildFacesForTet10_ ();
  }
  else if  ( elemType_ == 5 )   // 8 node hexahedron
  {
    buildFacesForHex8_ ();
  }
  else if  ( elemType_ == 17 )  // 20 node hexahedron
  {
    buildFacesForHex20_ ();
  }
  else
  {
    cerr << "Unsupported element type\n";
    exit(1);
  }
}


// ------------------------------------------------------------
//     isOnInterface
// ------------------------------------------------------------


bool Element::isOnInterface 

   ( IntVector&        face,
     int&              oppVertex,
     int&              fIndex,
     const NodeSet&    nodeSet,
           Int2IntMap& position ) const
{
  int         faceCount = faces_.   size ( );
  int         nodeCount = faces_[0].size ( );
  int         id, pos, mat;
  int         count;

  if ( !isInterfaceElement ( nodeSet, position ) )
  {
    return false;
  }

  IntVector   vertices(nodeCount);

  // loop over faces of 3D element

  for ( int jf = 0; jf < faceCount; jf++ )
  {
    vertices = faces_[jf];

    count    = 0;

    // loop over nodes of face jf

    for ( int in = 0; in < nodeCount; in++ )
    {
      id  = vertices[in];
      pos = position[id];

      mat = nodeSet[pos]->getDuplicity ();

      // if this node is interfacial

      if ( mat == 2 ) count++;
    }

    // if all nodes of face jf are interfacial
    // then this face is on INTERFACE

    if ( count == nodeCount ) 
    {
      face.resize ( count );
      face      = vertices;
      oppVertex = oppositeVertices_[jf];

      fIndex    = jf;

      return true;
    }
  }

  return false;
}


// ------------------------------------------------------------
//   isInterfaceElement
// ------------------------------------------------------------

bool Element::isInterfaceElement 

         ( const NodeSet& nodeSet,
	   Int2IntMap& position ) const
{
  int nodeCount  = connectivity0_.size ();
      nodeCount *= isQuadratic_ ? 0.5 : 1;

  if ( elemType_ == 17 ) nodeCount = 8;
	 
  int nodeFace   = nodePerFace_;    
      //nodeFace  *= isQuadratic_ ? 0.5 : 1;

  int index, mat, count = 0;

  for ( int in = 0; in < nodeCount; in++ )
  {
    index = connectivity0_[in];
    mat   = nodeSet[position[index]]->getDuplicity ();
    if ( mat > 1 ) count++;
  }

  if ( count == nodeFace ) 
  {
    return true;
  }

  return false;
}

// -------------------------------------------------------
//   getFaces
// -------------------------------------------------------

void Element::getFaces 
      
         ( vector<IntVector> faces ) const
{
  faces.resize ( faces_.size () );
  faces = faces_;

  // sort faces

  for ( int jf = 0; jf < faces.size (); jf++ )
  {
    sort ( faces[jf].begin(), faces[jf].end() );
  }
}


// -------------------------------------------------------
//   getCornerConnectivity0
// -------------------------------------------------------
// node numbering for quadratic elements: 
// follow Gmsh rule: corner nodes midside nodes

void  Element::getCornerConnectivity0

    ( IntVector& connec ) const
{
  if ( !isQuadratic_ )
  {
    getConnectivity0 ( connec );  
  }
  else
  {
    int count = connectivity0_.size () / 2;
    connec.resize ( count );

    for ( int i = 0; i < count; i++ )
    {
      connec[i] = connectivity0_[i];
    }
  }
}

void  Element::getCornerConnectivity

    ( IntVector& connec ) const
{
  if ( !isQuadratic_ )
  {
    getConnectivity ( connec );  
  }
  else
  {
    int count = connectivity_.size () / 2;
    connec.resize ( count );

    for ( int i = 0; i < count; i++ )
    {
      connec[i] = connectivity_[i];
    }
  }
}

// -------------------------------------------------------
//   getCornerConnectivity0
// -------------------------------------------------------

void Element::getBoundsOfFace

         ( pair<double,double>& xBound,
           pair<double,double>& yBound,
           pair<double,double>& zBound,
	   int                  fIndex,
	   Global&              globdat) const
{
  IntVector      vertices ( nodePerFace_ );
  vector<double> xCoord, yCoord, zCoord;

  double         x,y,z;
  int            id, pos;

  vertices = faces_[fIndex];

  for ( int in = 0; in < nodePerFace_; in++ )
  {
    id  = vertices[in];
    pos = globdat.nodeId2Position[id];

    x = globdat.nodeSet[pos]->getX();
    y = globdat.nodeSet[pos]->getY();
    z = globdat.nodeSet[pos]->getZ();

    xCoord.push_back ( x );
    yCoord.push_back ( y );
    zCoord.push_back ( z );
  }

  using boost::minmax_element;

  const pair<iter, iter> xbound = boost::minmax_element ( xCoord.begin(), xCoord.end  () );

  xBound.first  = *xbound.first;
  xBound.second = *xbound.second;

  const pair<iter, iter> ybound = boost::minmax_element ( yCoord.begin(), yCoord.end  () );

  yBound.first  = *ybound.first;
  yBound.second = *ybound.second;

  const pair<iter, iter> zbound = boost::minmax_element ( zCoord.begin(), zCoord.end  () );

  zBound.first  = *zbound.first;
  zBound.second = *zbound.second;
}

// -------------------------------------------------------
//   getJemConnectivity
// -------------------------------------------------------

void Element::getJemConnectivity

         ( IntVector& connec )            const
{
  connec.resize ( connectivity_.size() );

  if      ( !isQuadratic_ || isNURBS_ )
  {
    getJemConnectLinear_ ( connec );
  }
  else 
  {
    if      ( elemType_ == 11 )
    {
      getJemConnectTet10_ ( connec );
    }
    else if ( elemType_ == 17 )
    {
      getJemConnectHex20_ ( connec );
    }
    else
    {
      getJemConnect2DQuadratic_ ( connec );
    }
  }
}

// -------------------------------------------------------
//   PrintElement
// -------------------------------------------------------

struct PrintElement : public unary_function<ElemPointer,void>
{
         PrintElement ( ofstream& out ) : of(out){}

  void   operator () ( const ElemPointer ep ) const
  {
    IntVector connec;
    ep->getConnectivity ( connec );

    copy ( connec.begin(), connec.end(), 
	   ostream_iterator<int> (of, " ") );

    of << endl;

  }

  ofstream&  of;
};

// ----------------------------------------------------------
//   buildFacesForTet4
// ----------------------------------------------------------

void Element::buildFacesForTet4_ ()
{
  faces_.resize ( 4 );
  oppositeVertices_.resize ( 4 );

  faces_[0].resize ( 3 );
  faces_[1].resize ( 3 );
  faces_[2].resize ( 3 );
  faces_[3].resize ( 3 );

  faces_[0][0] = connectivity_[0];
  faces_[0][1] = connectivity_[1];
  faces_[0][2] = connectivity_[3];
  oppositeVertices_[0] = connectivity_[2];

  faces_[1][0] = connectivity_[1];
  faces_[1][1] = connectivity_[2];
  faces_[1][2] = connectivity_[3];
  oppositeVertices_[1] = connectivity_[0];

  faces_[2][0] = connectivity_[0];
  faces_[2][1] = connectivity_[2];
  faces_[2][2] = connectivity_[3];
  oppositeVertices_[2] = connectivity_[1];

  faces_[3][0] = connectivity_[0];
  faces_[3][1] = connectivity_[1];
  faces_[3][2] = connectivity_[2];
  oppositeVertices_[3] = connectivity_[3];

  nodePerFace_ = 3;
}

// ----------------------------------------------------------
//   buildFacesForTet10
// ----------------------------------------------------------

void Element::buildFacesForTet10_ ()
{
  faces_. resize ( 4 );
  ffaces_.resize ( 4 );
  oppositeVertices_.resize ( 4 );

  faces_[0].resize ( 3 ); faces_[1].resize ( 3 );
  faces_[2].resize ( 3 ); faces_[3].resize ( 3 );

  faces_[0][0] = connectivity_[0];
  faces_[0][1] = connectivity_[1];
  faces_[0][2] = connectivity_[3];
  oppositeVertices_[0] = connectivity_[2];

  ffaces_[0].resize ( 6 );
  ffaces_[0][0] = connectivity_[0]; ffaces_[0][3] = connectivity_[4];
  ffaces_[0][1] = connectivity_[1]; ffaces_[0][4] = connectivity_[9];
  ffaces_[0][2] = connectivity_[3]; ffaces_[0][5] = connectivity_[7];

  faces_[1][0] = connectivity_[1];
  faces_[1][1] = connectivity_[2];
  faces_[1][2] = connectivity_[3];
  oppositeVertices_[1] = connectivity_[0];

  ffaces_[1].resize ( 6 );
  ffaces_[1][0] = connectivity_[1]; ffaces_[1][3] = connectivity_[5];
  ffaces_[1][1] = connectivity_[2]; ffaces_[1][4] = connectivity_[8];
  ffaces_[1][2] = connectivity_[3]; ffaces_[1][5] = connectivity_[9];

  faces_[2][0] = connectivity_[0];
  faces_[2][1] = connectivity_[2];
  faces_[2][2] = connectivity_[3];
  oppositeVertices_[2] = connectivity_[1];

  ffaces_[2].resize ( 6 );
  ffaces_[2][0] = connectivity_[0]; ffaces_[2][3] = connectivity_[6];
  ffaces_[2][1] = connectivity_[2]; ffaces_[2][4] = connectivity_[8];
  ffaces_[2][2] = connectivity_[3]; ffaces_[2][5] = connectivity_[7];

  faces_[3][0] = connectivity_[0];
  faces_[3][1] = connectivity_[1];
  faces_[3][2] = connectivity_[2];
  oppositeVertices_[3] = connectivity_[3];

  ffaces_[3].resize ( 6 );
  ffaces_[3][0] = connectivity_[0]; ffaces_[3][3] = connectivity_[4];
  ffaces_[3][1] = connectivity_[1]; ffaces_[3][4] = connectivity_[5];
  ffaces_[3][2] = connectivity_[2]; ffaces_[3][5] = connectivity_[6];

  nodePerFace_ = 3;
}


// ----------------------------------------------------------
//   buildFacesForHex8
// ----------------------------------------------------------

void Element::buildFacesForHex8_ ()
{
  isQuadratic_ = false;

  faces_.           resize ( 6 );
  oppositeVertices_.resize ( 6 );

  faces_[0].resize ( 4 ); faces_[1].resize ( 4 ); faces_[2].resize ( 4 );
  faces_[3].resize ( 4 ); faces_[4].resize ( 4 ); faces_[5].resize ( 4 );

  faces_[0][0] = connectivity_[0];
  faces_[0][1] = connectivity_[3];
  faces_[0][2] = connectivity_[2];
  faces_[0][3] = connectivity_[1];
  oppositeVertices_[0] = connectivity_[4];

  faces_[1][0] = connectivity_[4];
  faces_[1][1] = connectivity_[7];
  faces_[1][2] = connectivity_[6];
  faces_[1][3] = connectivity_[5];
  oppositeVertices_[1] = connectivity_[0];

  faces_[2][0] = connectivity_[4];
  faces_[2][1] = connectivity_[0];
  faces_[2][2] = connectivity_[1];
  faces_[2][3] = connectivity_[5];
  oppositeVertices_[2] = connectivity_[7];

  faces_[3][0] = connectivity_[7];
  faces_[3][1] = connectivity_[6];
  faces_[3][2] = connectivity_[2];
  faces_[3][3] = connectivity_[3];
  oppositeVertices_[3] = connectivity_[4];

  faces_[4][0] = connectivity_[1];
  faces_[4][1] = connectivity_[5];
  faces_[4][2] = connectivity_[6];
  faces_[4][3] = connectivity_[2];
  oppositeVertices_[4] = connectivity_[0];

  faces_[5][0] = connectivity_[3];
  faces_[5][1] = connectivity_[0];
  faces_[5][2] = connectivity_[4];
  faces_[5][3] = connectivity_[7];
  oppositeVertices_[5] = connectivity_[2];

  nodePerFace_ = 4;
}

// ----------------------------------------------------------
//   buildFacesForHex20
// ----------------------------------------------------------

void Element::buildFacesForHex20_ ()
{
  faces_.           resize ( 6 );
  ffaces_.          resize ( 6 );
  oppositeVertices_.resize ( 6 );

  faces_[0].resize ( 4 ); faces_[1].resize ( 4 ); faces_[2].resize ( 4 );
  faces_[3].resize ( 4 ); faces_[4].resize ( 4 ); faces_[5].resize ( 4 );

  faces_[0][0] = connectivity_[0];
  faces_[0][1] = connectivity_[3];
  faces_[0][2] = connectivity_[2];
  faces_[0][3] = connectivity_[1];
  oppositeVertices_[0] = connectivity_[4];

  ffaces_[0].resize ( 8 );
  ffaces_[0][0] = connectivity_[0]; ffaces_[0][4] = connectivity_[8];
  ffaces_[0][1] = connectivity_[1]; ffaces_[0][5] = connectivity_[11];
  ffaces_[0][2] = connectivity_[2]; ffaces_[0][6] = connectivity_[13];
  ffaces_[0][3] = connectivity_[3]; ffaces_[0][7] = connectivity_[9];

  faces_[1][0] = connectivity_[4];
  faces_[1][1] = connectivity_[7];
  faces_[1][2] = connectivity_[6];
  faces_[1][3] = connectivity_[5];
  oppositeVertices_[1] = connectivity_[0];

  ffaces_[1].resize ( 8 );
  ffaces_[1][0] = connectivity_[4]; ffaces_[1][4] = connectivity_[16];
  ffaces_[1][1] = connectivity_[5]; ffaces_[1][5] = connectivity_[18];
  ffaces_[1][2] = connectivity_[6]; ffaces_[1][6] = connectivity_[19];
  ffaces_[1][3] = connectivity_[7]; ffaces_[1][7] = connectivity_[17];

  faces_[2][0] = connectivity_[4];
  faces_[2][1] = connectivity_[0];
  faces_[2][2] = connectivity_[1];
  faces_[2][3] = connectivity_[5];
  oppositeVertices_[2] = connectivity_[7];

  ffaces_[2].resize ( 8 );
  ffaces_[2][0] = connectivity_[4]; ffaces_[2][4] = connectivity_[16];
  ffaces_[2][1] = connectivity_[5]; ffaces_[2][5] = connectivity_[12];
  ffaces_[2][2] = connectivity_[1]; ffaces_[2][6] = connectivity_[8];
  ffaces_[2][3] = connectivity_[0]; ffaces_[2][7] = connectivity_[10];

  faces_[3][0] = connectivity_[7];
  faces_[3][1] = connectivity_[6];
  faces_[3][2] = connectivity_[2];
  faces_[3][3] = connectivity_[3];
  oppositeVertices_[3] = connectivity_[4];

  ffaces_[3].resize ( 8 );
  ffaces_[3][0] = connectivity_[7];  ffaces_[3][4] = connectivity_[19];
  ffaces_[3][1] = connectivity_[6];  ffaces_[3][5] = connectivity_[14];
  ffaces_[3][2] = connectivity_[2];  ffaces_[3][6] = connectivity_[13];
  ffaces_[3][3] = connectivity_[3];  ffaces_[3][7] = connectivity_[15];

  faces_[4][0] = connectivity_[1];
  faces_[4][1] = connectivity_[5];
  faces_[4][2] = connectivity_[6];
  faces_[4][3] = connectivity_[2];
  oppositeVertices_[4] = connectivity_[0];

  ffaces_[4].resize ( 8 );
  ffaces_[4][0] = connectivity_[1]; ffaces_[4][4] = connectivity_[11];
  ffaces_[4][1] = connectivity_[2]; ffaces_[4][5] = connectivity_[14];
  ffaces_[4][2] = connectivity_[6]; ffaces_[4][6] = connectivity_[18];
  ffaces_[4][3] = connectivity_[5]; ffaces_[4][7] = connectivity_[12];

  faces_[5][0] = connectivity_[3];
  faces_[5][1] = connectivity_[0];
  faces_[5][2] = connectivity_[4];
  faces_[5][3] = connectivity_[7];
  oppositeVertices_[5] = connectivity_[2];

  ffaces_[5].resize ( 8 );
  ffaces_[5][0] = connectivity_[3]; ffaces_[5][4] = connectivity_[15];
  ffaces_[5][1] = connectivity_[7]; ffaces_[5][5] = connectivity_[17];
  ffaces_[5][2] = connectivity_[4]; ffaces_[5][6] = connectivity_[10];
  ffaces_[5][3] = connectivity_[0]; ffaces_[5][7] = connectivity_[9];

  nodePerFace_ = 4;
}

// ----------------------------------------------------
//    getJemConnect2DQuadratic_
// ----------------------------------------------------

// Jemjive format: nodes (corner and midside) are continuous

void Element::getJemConnect2DQuadratic_

     ( IntVector& connect )            const
{

  int  halfNodeCount = connect.size () / 2;

  // print(connectivity_.begin(),connectivity_.end());

  for ( int i = 0; i < halfNodeCount; i++ )
  {
    connect[2*i]   = connectivity_[i];
    connect[2*i+1] = connectivity_[halfNodeCount+i];
  }
  // print(connect.begin(),connect.end());
}


// ----------------------------------------------------
//    getJemConnectTet10_
// ----------------------------------------------------


void Element::getJemConnectTet10_

     ( IntVector& connect )            const
{
  connect[0] = connectivity_[0]; connect[3] = connectivity_[9];
  connect[1] = connectivity_[7]; connect[4] = connectivity_[1];
  connect[2] = connectivity_[3]; connect[5] = connectivity_[4];

  connect[6] = connectivity_[6];
  connect[7] = connectivity_[8];
  connect[8] = connectivity_[5];

  connect[9] = connectivity_[2];
}


// ----------------------------------------------------
//    getJemConnectHex20_
// ----------------------------------------------------

void Element::getJemConnectHex20_

     ( IntVector& connect )            const
{
  connect[0]  = connectivity_[0];  connect[4] = connectivity_[5];
  connect[1]  = connectivity_[10]; connect[5] = connectivity_[12];
  connect[2]  = connectivity_[4];  connect[6] = connectivity_[1];
  connect[3]  = connectivity_[16]; connect[7] = connectivity_[8];

  connect[8]  = connectivity_[9];  connect[10] = connectivity_[18];
  connect[9]  = connectivity_[17]; connect[11] = connectivity_[11];

  connect[12] = connectivity_[3];  connect[16] = connectivity_[6];
  connect[13] = connectivity_[15]; connect[17] = connectivity_[14];
  connect[14] = connectivity_[7];  connect[18] = connectivity_[2];
  connect[15] = connectivity_[19]; connect[19] = connectivity_[13];
}

