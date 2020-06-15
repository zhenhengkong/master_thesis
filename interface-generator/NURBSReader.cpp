#include "Global.h"
#include "Node.h"
#include "Element.h"


#include <boost/algorithm/string.hpp>


void                     readNURBSMesh 

    ( Global&     globdat,
      const char* fileName )
{
  ifstream file ( fileName, std::ios::in );

  if ( !file ) 
  {
    cout << "Unable to open mesh file!!!\n\n";
    exit(1);
  }

  int             id;
  int             length;
  int             matId;
  int             elemType, paraElemType;

  double          x,y,z(0);

  StrVector       splitLine;
  IntVector       connectivity;

  string  line;

  cout << "Reading NURBS  mesh file ...\n";
  cout << "Reading nodes...\n";

  getline ( file, line );
  boost::split ( splitLine, line, boost::is_any_of("\t ") );

  int nodeCount  = boost::lexical_cast<int> ( splitLine[1] );
  int elemCount  = boost::lexical_cast<int> ( splitLine[3] );	

  getline ( file, line );

  for ( int in = 0; in < nodeCount; in++ )
  {
    file >> id >> x >> y;

    globdat.nodeSet.push_back ( NodePointer( new Node(x,y,z,id) ) );

    globdat.nodeId2Position[id] = globdat.nodeSet.size()-1;
  }

  // checking two or three dimensional mesh

  for ( int in = 0; in < nodeCount; in++ )
  {
    if ( globdat.nodeSet[in]->getZ() != 0. )
    {
      globdat.is3D = true;
      break;
    }
  }

  cout << "Reading nodes...done!\n\n";
  cout << "Reading elements...\n";


  getline ( file, line );
  getline ( file, line );

  for ( int ie = 0; ie < elemCount; ie++ )
  {
    getline ( file, line );

    boost::split ( splitLine, line, boost::is_any_of("\t ") );

    length       = splitLine.size ();

    // read connectivity 

    connectivity.clear ();

    transform ( splitLine.begin(), 
	        splitLine.end  ()-1, 
		back_inserter(connectivity), 
		Str2IntFunctor() );

    globdat.elemSet.push_back ( ElemPointer ( new Element ( ie, elemType, connectivity, true ) )  );

    globdat.elemId2Position[ie] = globdat.elemSet.size() - 1;
  }

  cout << "Reading elements...done!\n\n";
  cout << "Reading materials...\n";

  // material => elements
  // element  => material (domain)
  
  getline ( file, line );
  
  for ( int ie = 0; ie < elemCount; ie++ )
  {
    getline ( file, line );

    boost::split ( splitLine, line, boost::is_any_of("\t ") );

    matId  = boost::lexical_cast<int> ( splitLine[1] );

    globdat.dom2Elems[matId].push_back ( ie );
    globdat.elem2Domain[ie] = matId;
  }

  cout << "Reading materials...done!\n\n";

  cout << "Reading node groups...\n";

  getline ( file, line );

  boost::split ( splitLine, line, boost::is_any_of("\t ") );

  int nodeGroupCount  = boost::lexical_cast<int> ( splitLine[2] );

  for (int ing = 0; ing < nodeGroupCount; ing++ )
  {
    getline ( file, line );
    getline ( file, line );
      
    boost::split ( splitLine, line, boost::is_any_of("\t ") );

    connectivity.clear ();

    transform ( splitLine.begin(), 
                splitLine.end  ()-1, 
          	back_inserter(connectivity), 
          	Str2IntFunctor() );
    
    for ( int ii = 0; ii < connectivity.size(); ii++)
    {
      globdat.boundaryNodes.insert ( connectivity[ii] );
    }

    for ( int ii = 0; ii < connectivity.size(); ii++)
    {
      globdat.bndNodesMap[ing].insert ( connectivity[ii] );
    }
  }

  cout << "Reading node groups...done!\n\n";

  file.close ();

  // check validity of input

  if ( globdat.isDomain )
  {
    Int2IntVectMap::const_iterator it;
    Int2IntVectMap::const_iterator eit = globdat.dom2Elems.end   ();

    it = globdat.dom2Elems.find ( globdat.rigidDomain );

    if ( it == eit )
    {
      cerr << "invalid number of rigid domain!!!\n";
      exit(1);
    }
  }

  if ( globdat.isInterface )
  {
  }

  if ( globdat.isNotch )
  {
    cout << "Existing notch segment is: " ;//<< globdat.segment << endl;
  }

  if ( globdat.isIgSegment )
  {
    cout << "Do not treat nodes on this segment: " << globdat.ignoredSegment << endl;
  }

  cout << endl;

  globdat.nodeElemCount = connectivity.size ();

  string elemTypeStr = "linear";

  elemType = globdat.elemSet[0]->getElemType ();

  if ( !globdat.is3D )
  {
    if ( elemType == 2 || elemType == 3 )
    {
      globdat.nodeICount  = globdat.isContinuum ? 4 : 2;
      globdat.isQuadratic = false;
      elemTypeStr         = "linear";
    }
    else
    {
      globdat.nodeICount  = 6;
      globdat.isQuadratic = true;
      elemTypeStr         = "quadratic";
    }
  }
  else
  {
    if ( elemType == 4 || elemType == 5 ) 
    {
      globdat.nodeICount  = elemType == 4 ? 6 : 8;
      globdat.isQuadratic = false; 
      elemTypeStr = "linear";
    }
    else
    {
      globdat.nodeICount  = elemType == 11 ? 12 : 16;
      globdat.isQuadratic = true;
      elemTypeStr = "quadratic";
    }

    for ( int ie = 0; ie < globdat.elemSet.size (); ie++ )
    {
      globdat.elemSet[ie]->buildFaces ();
    }
  }


}
