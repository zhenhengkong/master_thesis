
#include <algorithm>
#include <iterator>

#include "InterfaceBuilder.h"
#include "Global.h"
#include "Element.h"
#include "Node.h"
#include "utilities.h"

// ---------------------------------------------------------
//   doIt
// ---------------------------------------------------------

void   InterfaceBuilder::doIt ( Global& globdat )
{
  cout << "Adding interface elements...\n";

  if      ( globdat.isInterface )
  {
    cout << " - for material interfaces..\n";
    doForMatInterface ( globdat );

    goto inform;
  }
  else if ( globdat.isDomain )
  {
    doForDomain       ( globdat ); 

    goto inform;
  }
  else if ( globdat.isPolycrystal )
  {
    doForPolycrystal  ( globdat );

    goto inform;
  }

  assert ( globdat.interfaceSet.size () != 0 );

inform:
  cout << "Adding interface elements...done!\n\n";

  cout << "Number of interface elements added:  " << globdat.interfaceSet.size () << endl
       << "Number of nodes added             :  " << globdat.interfaceSet.size () << endl
       << "Number of nodes added             :  " << globdat.newNodeSet.size () - globdat.nodeSet.size() << endl
       << "\n\n";

}

// ---------------------------------------------------------
//   doForMatInterface
// ---------------------------------------------------------

void   InterfaceBuilder::doForMatInterface ( Global& globdat )
{
  if      ( globdat.is3D )
  {
    doFor3DMatInterface ( globdat );
  }
  else if (globdat.isContinuum) 
  {
    if (!globdat.isNURBS)  doFor2DMatInterface ( globdat );
    else                   doFor2DMatInterfaceNURBS (globdat);
  }
  else
  {
    addDiscreteInterface ( globdat );
  }
}

// ---------------------------------------------------------
//   doForPolycrystal
// ---------------------------------------------------------

void   InterfaceBuilder::doForPolycrystal ( Global& globdat )
{
  if ( globdat.is3D )
  {
    doFor3DPolycrystal ( globdat );
  }
  else
  {
    doFor2DPolycrystal ( globdat );
  }

}

// ---------------------------------------------------------
//   doFor2DMatInterface
// ---------------------------------------------------------

void   InterfaceBuilder::doFor2DMatInterface ( Global& globdat )
{
  int              n1,n2,p1;
  int              m1,m2;
  int              o1,o2;
  int              ielem,  ieCount = 0;
  int              nnode;

  ElemPointer      ep;
  
  IntVector        inodes0, inodesF;
  IntVector        interConnec(globdat.nodeICount);
  IntVector        interConnec1, interConnec2;

  vector<NodePair> doneEdges;   // list of edges already done
  set<int>         doneNodes;   // list of edges already done
  doneNodes.insert(-1);

  const  int       elemCount = globdat.elemSet.size ();

  for ( int ie = 0; ie < elemCount; ie++ )
  {
     ep    = globdat.elemSet[ie];

     ep->getCornerConnectivity0 ( inodes0 );

     if ( globdat.isQuadratic ) 
     {
       ep->getConnectivity0 ( inodesF ); 
     }

     inodes0.push_back ( inodes0[0] );  

     nnode = inodes0.size(); 

     // loop over edges, add interface along common edge

     for ( int in = 0; in < nnode-1; in++ )
     {
       n1 = inodes0[in];
       n2 = inodes0[in+1];

       if ( globdat.isQuadratic ) p1 = inodesF[nnode - 1 + in]; // index of midside node

       o1 = globdat.nodeId2Position[n1];
       o2 = globdat.nodeId2Position[n2];

       m1 = globdat.nodeSet[o1]->getDuplicity ();
       m2 = globdat.nodeSet[o2]->getDuplicity ();

       // not a common edge, omits

       if ( ( m1 == 0 ) || ( m2 == 0 ) ) continue;
       if ( ( m1 == 1 ) || ( m2 == 1 ) ) continue;

       // edge on external boundary, also omitted

       if ( find ( globdat.nodePairs.begin(), 
                          globdat.nodePairs.end  (), 
                          NodePair(n1,n2) ) != globdat.nodePairs.end() )
       {
         continue;
       }

       // ignore edge already added

       if ( find ( doneEdges.begin(), 
                          doneEdges.end  (), 
                          NodePair(n1,n2) ) != doneEdges.end() )
       {
         break;
       }

       // existing notch

       if ( globdat.isNotch )
       {
         bool val1, val2;

         double x1 = globdat.nodeSet[o1]->getX ();
         double y1 = globdat.nodeSet[o1]->getY ();

         double x2 = globdat.nodeSet[o2]->getX ();
         double y2 = globdat.nodeSet[o2]->getY ();

         for ( int is = 0; is < globdat.segment.size(); is++ )
         {
           val1 = globdat.segment[is].isOn ( x1, y1 );
           val2 = globdat.segment[is].isOn ( x2, y2 );

           if ( val1 && val2 ) break;
         }

         if ( val1 && val2 ) break; // do not add interface on existing notch
       }

       addInterface ( interConnec, 
                      n1, n2, p1, ep->getChanged(), globdat );

       globdat.interfaceSet.push_back ( ElemPointer ( new Element ( ieCount, interConnec ) ) );
       
       globdat.interfaceMats.push_back (0);
       ieCount++;

       doneEdges.push_back ( NodePair(n1,n2) );
    }
  }
}

// ---------------------------------------------------------
//   doFor3DMatInterface
// ---------------------------------------------------------

void   InterfaceBuilder::doFor3DMatInterface ( Global& globdat )
{
  ElemPointer        ip, jp;

  IntVector          face, sface, fface;
  IntVector          interConnec(globdat.nodeICount);
  IntVector          neighbors;
  vector<IntVector>  doneFaces;

  int                nodeCount, neiCount;
  int                ieCount = 0;
  int                ielem, jelem, n, m;
  int                oppVertex, fIndex;

  const  int         elemCount = globdat.elemSet.size ();

  for ( int ie = 0; ie < elemCount; ie++ )
  {
    ip    = globdat.elemSet[ie];
    ielem = ip->getIndex ();

    if ( ip->isOnInterface ( face, oppVertex, fIndex,
                             globdat.nodeSet, globdat.nodeId2Position ) )
    {
      //cout << "found one interface \n";
      //print ( face.begin(), face.end() );

      // do not write interface elements on existing 
      // notches. Current implementation only deals
      // with horizontal existing notches.

      if ( globdat.isNotch )
      {
        bool          ignored = false;

        pair<double,double> xBound, yBound, zBound;

        ip->getBoundsOfFace ( xBound, yBound, zBound, 
                              fIndex, globdat );

        double xMin, xMax, Z;

        vector<double> zCoord(2); zCoord[0] = 0.; zCoord[1]=-0.265;

        for ( int is = 0; is < globdat.segment.size(); is++ )
        {
          xMin = globdat.segment[is].p1.x; 
          xMax = globdat.segment[is].p2.x;
          Z    = zCoord[is];

          if ( xBound.second < xMax && xBound.second >= xMin && 
               yBound.first == Z )
          {	
            ignored = true; 
            break;
          }
        }

        if (ignored) continue;
      }

      sface = face;

      sort ( sface.begin(), sface.end() );

      if ( find ( doneFaces.begin(),
                  doneFaces.end  (), 
        	  sface ) != doneFaces.end() )
      {
        continue;
      }

      // determine connectivity of this interface

      if ( globdat.isQuadratic ) 
      {
        ip->getFullFace ( fface, fIndex );

        nodeCount  = fface.size ();

        for ( int in = 0; in < nodeCount/2; in++ )
        {
          n                           = fface[in];
          m                           = fface[in+nodeCount/2];

          interConnec[2*in]             = n;
          interConnec[2*in+1]           = m;

          interConnec[2*in+nodeCount]   = globdat.duplicatedNodes0[n][1] ;
          interConnec[2*in+1+nodeCount] = globdat.duplicatedNodes0[m][1] ;
        }
      }
      else
      {
        nodeCount  = face.size ();

        for ( int in = 0; in < nodeCount; in++ )
        {
          n                          = face[in];
          interConnec[in]            = n;
          interConnec[in+nodeCount]  = globdat.duplicatedNodes0[n][1] ;
        }
      }

      // insert this interface 

      globdat.interfaceSet.push_back 
        ( ElemPointer ( new Element ( ieCount, interConnec ) ) );

      globdat.interfaceMats.push_back (0);
      globdat.oppositeVertices.push_back ( oppVertex );
      ieCount++;

      doneFaces.push_back ( sface );
    }
  }
}

// ---------------------------------------------------------
//   doForDomain
// ---------------------------------------------------------

void   InterfaceBuilder::doForDomain ( Global& globdat )
{
  int              neiCount;
  int              nnode;
  int              n1,n2;
  int              m1,m2;
  int              o1,o2;
  int              p1,p2;
  int              ielem, jelem, ieCount = 0;

  ElemPointer      ep,jp;
  
  IntVector        inodes, inodes0, inodes00;
  IntVector        jnodes, jnodes0;
  IntVector        interConnec(4);
  IntVector        neighbors;
  vector<NodePair> doneEdges;   // list of edges already done


  IntVector::const_iterator it1, it2;

  const  int        elemCount = globdat.elemSet.size ();

  for ( int ie = 0; ie < elemCount; ie++ )
  {
    ep    = globdat.elemSet[ie];
    ielem = ep->getIndex();

    ep->getConnectivity  ( inodes  ); 
    ep->getConnectivity0 ( inodes0 ); 
    inodes00 = inodes0;
    
    inodes .push_back ( inodes [0] );  
    inodes0.push_back ( inodes0[0] );  

    nnode = inodes0.size();

    neighbors = globdat.elemNeighbors[ie]; 
    neiCount  = neighbors.size ();
    
    cout << "doing element " << ielem << ": ";
   // print ( neighbors.begin(), neighbors.end() );

    // loop over edges, add interface along common edge

    for ( int in = 0; in < nnode-1; in++ )
    {
      n1 = inodes0[in];
      n2 = inodes0[in+1];

      o1 = globdat.nodeId2Position[n1];
      o2 = globdat.nodeId2Position[n2];

      m1 = globdat.nodeSet[o1]->getDuplicity ();
      m2 = globdat.nodeSet[o2]->getDuplicity ();

      // not a common edge, omits
      
      if ( ( m1 == 0 ) || ( m2 == 0 ) ) continue;
      if ( ( m1 == 1 ) || ( m2 == 1 ) ) continue;

      // edge on external boundary, also omitted

      if ( find ( globdat.nodePairs.begin(), 
        	  globdat.nodePairs.end  (), 
        	  NodePair(n1,n2) ) != globdat.nodePairs.end() )
      {
        continue;
      }

      // ignore edge already added

      if ( find ( doneEdges.begin(), 
        	  doneEdges.end  (), 
        	  NodePair(n1,n2) ) != doneEdges.end() )
      {
        continue;
      }

      cout << "edge (" << n1 << "," << n2 << ")" << endl;

      int mat = globdat.elem2Domain[ielem];

      if ( ( mat == globdat.rigidDomain ) || ( inodes == inodes00 ) )
      {
        interConnec[0] = n1;
        interConnec[1] = n2;
      }
      else
      {
        interConnec[0] = inodes[in];
        interConnec[1] = inodes[in+1];
      }

      // loop over neighbors of element ie

      for ( int je = 0; je < neiCount; je++ )
      {
         jelem = globdat.elemId2Position[neighbors[je]];
         jp    = globdat.elemSet[jelem];

         if ( jp->getIndex() == ielem ) 
         {
           continue; 
         }

         jp->getConnectivity  ( jnodes  );
         jp->getConnectivity0 ( jnodes0 );

         //print ( jnodes0.begin(), jnodes0.end() );

         it1 = find ( jnodes0.begin(), jnodes0.end(), n1 );
         it2 = find ( jnodes0.begin(), jnodes0.end(), n2 );

         if ( ( it1 != jnodes0.end() ) && 
              ( it2 != jnodes0.end() ) )
         {
           p1 = jnodes[it1 - jnodes0.begin()];
           p2 = jnodes[it2 - jnodes0.begin()];

           cout << " found common edge: (" << p1 << "," << p2 << ")" << endl;

           interConnec[2] = p1;
           interConnec[3] = p2;

           break; // only have ONE edge in common
         }
      }

      globdat.interfaceSet.push_back ( ElemPointer ( new Element ( ieCount, interConnec ) )
        		     );

      if ( ( globdat.nodeSet[o1]->getIsInterface() ) && 
           ( globdat.nodeSet[o2]->getIsInterface() ) )
      {
        globdat.interfaceMats.push_back ( 1 );
      }
      else
      {
        globdat.interfaceMats.push_back ( 0 );
      }

      doneEdges.push_back ( NodePair(n1,n2) );

      ieCount++;
    }
  }
}

// ---------------------------------------------------------
//   doFor2DPolycrystal
// ---------------------------------------------------------

void   InterfaceBuilder::doFor2DPolycrystal ( Global& globdat )
{
  int              n1,n2,p12,n10,n20;
  int              p1,p2,m12;
  int              m1,m2;
  int              o1,o2;
  int              ielem, jelem;
  int        	   nnode, ieCount = 0;

  ElemPointer      ep, jp;
  
  IntVector        inodes0, inodes, inodesF;

  IntVector        interConnec(globdat.nodeICount);
  IntVector        neighbors;

  vector<NodePair> doneEdges;   

  int              ignoredEdgeCount = 0;

  const  int       elemCount = globdat.elemSet.size ();

   for ( int ie = 0; ie < elemCount; ie++ )
   {
      ep    = globdat.elemSet[ie];
      ielem = ep->getIndex();

      ep->getCornerConnectivity0 ( inodes0 ); 
      ep->getCornerConnectivity  ( inodes  ); 

      if ( globdat.isQuadratic ) 
      {
        ep->getConnectivity0 ( inodesF ); 
      }

      inodes0.push_back ( inodes0[0] );  
      inodes .push_back ( inodes [0] );  

      nnode     = inodes0.size();
 
      neighbors = globdat.elemNeighbors[ie]; 

      // loop over edges, add interface along common edge

      for ( int in = 0; in < nnode-1; in++ )
      {
        n10  = inodes0[in]; n20 = inodes0[in+1];
        n1   = inodes[in];  n2  = inodes[in+1];

        // index of midside node

        if ( globdat.isQuadratic ) p12 = inodesF[nnode - 1 + in]; 

        o1  = globdat.nodeId2Position[n10];
        o2  = globdat.nodeId2Position[n20];

        m1  = globdat.nodeSet[o1]->getDuplicity ();
        m2  = globdat.nodeSet[o2]->getDuplicity ();

        // not a common edge, omits
        
        if ( ( m1 == 0 ) || ( m2 == 0 ) ) continue;
        if ( ( m1 == 1 ) || ( m2 == 1 ) ) continue;

        // edge on external boundary, also omitted

        if ( find ( globdat.nodePairs.begin(), 
        	    globdat.nodePairs.end  (), 
        	    NodePair(n10,n20) ) != globdat.nodePairs.end() )
        {
          continue;
        }

        // ignore edge already added

        if ( find ( doneEdges.begin(), 
        	    doneEdges.end  (), 
        	    NodePair(n10,n20) ) != doneEdges.end() )
        {
          break;
        }

        // ignore edge belongs to ignoredEdges

        if ( find ( globdat.ignoredEdges.begin(), 
        	    globdat.ignoredEdges.end  (), 
        	    NodePair(n10,n20) ) != globdat.ignoredEdges.end() )
        {
          break;
        }

        // existing notch
        
        if ( globdat.isNotch )
        {
          double x1 = globdat.nodeSet[o1]->getX ();
          double y1 = globdat.nodeSet[o1]->getY ();

          double x2 = globdat.nodeSet[o2]->getX ();
          double y2 = globdat.nodeSet[o2]->getY ();

          bool val1 = globdat.segment[0].isOn ( x1, y1 );
          bool val2 = globdat.segment[0].isOn ( x2, y2 );
          
          if ( val1 && val2 )
          {
            cout << n1 << " p1 (" << x1 << "," << y1 << ")\n";
            cout << n2 << " p2 (" << x2 << "," << y2 << ")\n\n";

            doneEdges.push_back ( NodePair(n10,n20) );
            ignoredEdgeCount++;

            break;
          }
        }
        
        // for nodes along polycrystal boundary but not at the junction point

        if       ( m1 == 2 && m2 == 2 )
        {
          addInterface ( interConnec, n10, n20, p12, ep->getChanged(), globdat );        
        }
        else if  ( m1 == 3 || m2 == 3 ) // junction node
        {
          findCommonEdge ( p1, p2, n10, n20, ielem, neighbors, globdat );

          if ( ep->getChanged() )
          {
            if ( !globdat.isQuadratic )
            {
              interConnec[0] = n10 == n1 ? n10 : n1;
              interConnec[1] = n20 == n2 ? n20 : n2;
            }
            else
            {	    
              interConnec[0] = n10 == n1 ? n10 : n1;
              interConnec[2] = n20 == n2 ? n20 : n2;
            }
          }

          addInterface   ( interConnec, n10, n20, p12, 
                           p1, p2, ep->getChanged(), globdat );	
        }
        else
        {
          cerr << "Impossible for this case to happen!!!\n";
          exit(1);
        }

        globdat.interfaceSet.push_back ( ElemPointer ( new Element ( ieCount, interConnec ) )
        			       );

        globdat.interfaceMats.push_back (0);
        ieCount++;
        doneEdges.push_back ( NodePair(n1,n2) );
     }
  }

  cout << "Number of ignored interfaces: " << ignoredEdgeCount << endl; 

}

// ---------------------------------------------------------
//   doFor3DPolycrystal
// ---------------------------------------------------------

void   InterfaceBuilder::doFor3DPolycrystal ( Global& globdat )
{
   ElemPointer        ip;

  IntVector          face, sface;
  IntVector          interConnec;
  IntVector          neighbors;
  IntVector          inodes, inodes0;

  vector<IntVector>  doneFaces;

  int                nodeCount, neiCount;
  int                ieCount = 0;
  int                ielem, jelem, m, n, mat;
  int                oppVertex, fIndex;

  bool               isOnInterface;
  bool               isJunction = false;

  const  int         elemCount = globdat.elemSet.size ();

  for ( int ie = 0; ie < elemCount; ie++ )
  {
    ip    = globdat.elemSet[ie];
    ielem = ip->getIndex ();

    isOnInterface = ip->isOnInterface ( face, oppVertex, fIndex, 
                                        globdat.nodeSet, globdat.nodeId2Position );

    if ( !isOnInterface ) continue;

    ip->getConnectivity  ( inodes  );
    ip->getConnectivity0 ( inodes0 );

    if ( isOnInterface )
    {
      // there are two cases: 
      // (1) bimaterial face
      // (2) trimaterial face (there're nodes at the junction)

      nodeCount  = face.size ();
      interConnec.resize ( nodeCount * 2 );

      for ( int in = 0; in < nodeCount; in++ )
      {
        m    = face[in];
        mat  = globdat.nodeSet[globdat.nodeId2Position[m]]->getDuplicity();

        if ( mat == 3 ) 
        {
          isJunction = true;
          break;
        }
      }

      if ( globdat.isNotch )
      {
        double xMax = globdat.segment[0].p2.x;

        int    n1   = face[0];
        int    n2   = face[1];

        double x1   = globdat.nodeSet[globdat.nodeId2Position[n1]]->getX ();
        double x2   = globdat.nodeSet[globdat.nodeId2Position[n2]]->getX ();

        double x = x1 > x2 ? x1 : x2;

        if ( x < xMax ) continue;
      }

      // sort the connectivity of face so that
      // it can be compared with simply "=" operator

      sface = face;
      sort ( sface.begin(), sface.end() );

      if ( find ( doneFaces.begin(),
                  doneFaces.end  (), 
        	  sface ) != doneFaces.end() )
      {
        continue;
      }

      if ( !isJunction )
      {
        // determine connectivity of this interface

        for ( int in = 0; in < nodeCount; in++ )
        {
          n                          = face[in];
          interConnec[in]            = n;
          interConnec[in+nodeCount]  = globdat.duplicatedNodes0[n][1] ;
        }
      }
      else
      {
        IntVector           neighbors;
        IntVector           jnodes,jnodes0;
        vector<IntVector>   jfaces;

        int                 neiCount, jelem;
        int                 p1, p2, p3;
        int                 m1, m2, m3;

        ElemPointer         jp;

        IntVector::const_iterator 
          
                            it1, it2, it3, it0, itE;

        m1 = face[0];
        m2 = face[1];
        m3 = face[2];

        if ( inodes == inodes0 ) 
        {
          interConnec[0] = m1;
          interConnec[1] = m2;
          interConnec[2] = m3;
        }
        else
        {	     
          it0 = inodes0.begin();
          itE = inodes0.end  ();

          it1 = find ( it0, itE, m1 );
          it2 = find ( it0, itE, m2 );
          it3 = find ( it0, itE, m3 );

          interConnec[0] = inodes[it1-it0];
          interConnec[1] = inodes[it2-it0];
          interConnec[2] = inodes[it3-it0];
        }

        neighbors = globdat.elemNeighbors[ie]; 
        neiCount  = neighbors.size ();

        // loop over neighbors of element ie

        for ( int je = 0; je < neiCount; je++ )
        {
           jelem = globdat.elemId2Position[neighbors[je]];
           jp    = globdat.elemSet[jelem];

           // omit element being considered

           if ( jp->getIndex() == ielem ) continue; 

           // find common face

           jp->getFaces         ( jfaces  );
           jp->getConnectivity  ( jnodes  );
           jp->getConnectivity0 ( jnodes0 );

           // common face between ielem and jelem found

           if ( find ( jfaces.begin(), 
        	       jfaces.end  (), sface ) != jfaces.end() )
           {
             it0 = jnodes0.begin();
             itE = jnodes0.end  ();

             it1 = find ( it0, itE, m1 );
             it2 = find ( it0, itE, m2 );
             it3 = find ( it0, itE, m3 );

             p1 = jnodes[it1 - it0];
             p2 = jnodes[it2 - it0];
             p3 = jnodes[it2 - it0];

             interConnec[3] = p1;
             interConnec[4] = p2;
             interConnec[5] = p3;

             break; // only have ONE face in common
           }
        }
      }

      // insert this interface 

      globdat.interfaceSet.push_back 
        ( ElemPointer ( new Element ( ieCount, interConnec ) ) );

      globdat.interfaceMats.push_back    (0);
      globdat.oppositeVertices.push_back (oppVertex);
      ieCount++;

      doneFaces.push_back ( sface );
    }
  }
}

// ----------------------------------------------------------
//    addInterface
// ----------------------------------------------------------

// node numbering for quadratic elements
// 3-node line element: corner1 midside corner2
// Gmsh's convention is different: corner1 corner 2 midside !!!

void  addInterface

         ( IntVector& interConnec,
           int        n1,
           int        n2,
           int        p1,
           bool       changed,
           Global&    globdat )
{
  if ( !globdat.isQuadratic )
  {
    // handle orientation of the cohesive segment
    // the direction  from first node to second node
    // == left to right

    if ( !changed )
    {
      // add nodes for the two faces of the interface element

      interConnec[0] = globdat.duplicatedNodes0[n1][1];
      interConnec[1] = globdat.duplicatedNodes0[n2][1];

      interConnec[2] = globdat.duplicatedNodes0[n1][0];
      interConnec[3] = globdat.duplicatedNodes0[n2][0];
    }
    else
    {
      interConnec[0] = globdat.duplicatedNodes0[n2][1];
      interConnec[1] = globdat.duplicatedNodes0[n1][1];
      interConnec[2] = globdat.duplicatedNodes0[n2][0];
      interConnec[3] = globdat.duplicatedNodes0[n1][0];
    }
  }
  else
  {
    if ( !changed )
    {
      interConnec[0] = globdat.duplicatedNodes0[n1][1];
      interConnec[1] = globdat.duplicatedNodes0[p1][1]; // midside node
      interConnec[2] = globdat.duplicatedNodes0[n2][1];

      interConnec[3] = globdat.duplicatedNodes0[n1][0];
      interConnec[4] = globdat.duplicatedNodes0[p1][0]; // midside node
      interConnec[5] = globdat.duplicatedNodes0[n2][0];
    }
    else
    {
      interConnec[0] = globdat.duplicatedNodes0[n2][1];
      interConnec[1] = globdat.duplicatedNodes0[p1][1]; // midside node
      interConnec[2] = globdat.duplicatedNodes0[n1][1];

      interConnec[3] = globdat.duplicatedNodes0[n2][0];
      interConnec[4] = globdat.duplicatedNodes0[p1][0]; // midside node
      interConnec[5] = globdat.duplicatedNodes0[n1][0];
    }
  }
}

// ------------------------------------------------------------------
//    addDiscreteInterface
// ------------------------------------------------------------------

void  addDiscreteInterface

         ( Global&    globdat )
{
  const int   inodeCount = globdat.interfaceNodes.size();

  int         ieCount = 0;
  int         index, o1;
  IntVector   interConnec(2);
  
  for ( int i = 0; i < inodeCount; i++ )
  {
    index = globdat.interfaceNodes[i];

    o1    = globdat.nodeId2Position[index];
    
    if ( globdat.isNotch )
    {
      bool val1;

      double x1 = globdat.nodeSet[o1]->getX ();
      double y1 = globdat.nodeSet[o1]->getY ();

      for ( int is = 0; is < globdat.segment.size(); is++ )
      {
        val1 = globdat.segment[is].isOn ( x1, y1 );
        if ( val1 ) break;
      }

      if ( val1  ) continue; // do not add interface on existing notch
    } 
    
    interConnec[0] = globdat.duplicatedNodes0[index][0] ;
    interConnec[1] = globdat.duplicatedNodes0[index][1] ;
         
    globdat.interfaceSet.push_back ( ElemPointer ( new Element ( ieCount, interConnec ) ) );
       
    globdat.interfaceMats.push_back (0);
    ieCount++;
  }
}

// ---------------------------------------------------------
//   doFor2DMatInterfaceNURBS
// ---------------------------------------------------------

void   InterfaceBuilder::doFor2DMatInterfaceNURBS ( Global& globdat )
{
  cout << "   do for NURBS mesh \n";

  int              n1,n2,p1;
  int              m1,m2;
  int              o1,o2;
  int              ielem,  ieCount = 0;
  int              nnode;

  ElemPointer      ep;
  
  IntVector        connec, inodes0, edge1, edge2, edge3, edge4;
  IntVector        interConnec;
  IntVector        interConnec1, interConnec2;
  vector< vector<int> > edges;

  vector<NodePair> doneEdges;   // list of edges already done
  set<int>         doneNodes;   // list of edges already done
  doneNodes.insert(-1);

  const  int       elemCount = globdat.elemSet.size ();
         int       numNode;
         int       order;

  for ( int ie = 0; ie < elemCount; ie++ )
  {
     ep    = globdat.elemSet[ie];

     ep->getConnectivity0 ( connec );

     numNode = connec.size ();

     switch (numNode)
     {   
       case 9: // 9B elements

         edge1.push_back ( connec[0]);   edge1.push_back ( connec[1]); edge1.push_back ( connec[2]);   
         edge2.push_back ( connec[2]);   edge2.push_back ( connec[5]); edge2.push_back ( connec[8]);   
         edge3.push_back ( connec[6]);   edge3.push_back ( connec[7]); edge3.push_back ( connec[8]);   
         edge4.push_back ( connec[0]);   edge4.push_back ( connec[3]); edge4.push_back ( connec[6]);   


         edges.push_back ( edge1 );
         edges.push_back ( edge2 );
         edges.push_back ( edge3 );
         edges.push_back ( edge4 );

         globdat.nodeICount = 6;

         interConnec1.resize(3);
         
         inodes0.push_back ( connec[0] );
         inodes0.push_back ( connec[2] );
         inodes0.push_back ( connec[8] );
         inodes0.push_back ( connec[6] );
         inodes0.push_back ( inodes0[0] );
         
         order = 2;
         break;

       case 16: //4x4 elements (16B) 

         edge1.push_back ( connec[0]);   edge1.push_back ( connec[1]);
         edge1.push_back ( connec[2]);   edge1.push_back ( connec[3]);

         edge2.push_back ( connec[3]);   edge2.push_back ( connec[7]);
         edge2.push_back ( connec[11]);  edge2.push_back ( connec[15]);
         
         edge3.push_back ( connec[15]);  edge3.push_back ( connec[14]);
         edge3.push_back ( connec[13]);  edge3.push_back ( connec[12]);

         edge4.push_back ( connec[12]);  edge4.push_back ( connec[8]);
         edge4.push_back ( connec[4]);   edge4.push_back ( connec[0]);

         edges.push_back ( edge1 );
         edges.push_back ( edge2 );
         edges.push_back ( edge3 );
         edges.push_back ( edge4 );

         order = 3; 
         globdat.nodeICount = 8;

         interConnec1.resize(4);
         
         inodes0.push_back ( connec[0] );
         inodes0.push_back ( connec[order] );
         inodes0.push_back ( connec[(order-1)*(order+1) + order + 1 + order] );
         inodes0.push_back ( connec[(order-1)*(order+1) + order + 1] );
         inodes0.push_back ( inodes0[0] );
         
         break;
       
       case 6: //3x2 elements 

         edge1.push_back ( connec[0]);   edge1.push_back ( connec[1]); edge1.push_back ( connec[2]);   
         edge2.push_back ( connec[2]);   edge2.push_back ( connec[5]);
         edge3.push_back ( connec[5]);   edge3.push_back ( connec[4]); edge3.push_back ( connec[3]);  
         edge4.push_back ( connec[3]);   edge4.push_back ( connec[0]);

         edges.push_back ( edge1 );
         edges.push_back ( edge2 );
         edges.push_back ( edge3 );
         edges.push_back ( edge4 );

         order = 2; 
         globdat.nodeICount = 6;

         interConnec1.resize(3);

         inodes0.push_back ( connec[0] );
         inodes0.push_back ( connec[2] );
         inodes0.push_back ( connec[5] );
         inodes0.push_back ( connec[3] );
         inodes0.push_back ( inodes0[0] );

         break;

       case 8: //4x2 elements 

         edge1.push_back ( connec[0]);   edge1.push_back ( connec[1]);
         edge1.push_back ( connec[2]);   edge1.push_back ( connec[3]);

         edge2.push_back ( connec[3]);   edge2.push_back ( connec[7]);
         
         edge3.push_back ( connec[7]);  edge3.push_back ( connec[6]);
         edge3.push_back ( connec[5]);  edge3.push_back ( connec[4]);

         edge4.push_back ( connec[4]);   edge4.push_back ( connec[0]);

         edges.push_back ( edge1 );
         edges.push_back ( edge2 );
         edges.push_back ( edge3 );
         edges.push_back ( edge4 );

         order = 3; 
         globdat.nodeICount = 8;

         interConnec1.resize(4);

         inodes0.push_back ( connec[0] );
         inodes0.push_back ( connec[3] );
         inodes0.push_back ( connec[7] );
         inodes0.push_back ( connec[4] );
         inodes0.push_back ( inodes0[0] );

         break;

       case 12: //4x3 elements 

         edge1.push_back ( connec[0]);   edge1.push_back ( connec[1]); 
         edge1.push_back ( connec[2]);   edge1.push_back ( connec[3]);   

         edge2.push_back ( connec[3]);   edge2.push_back ( connec[7]); edge2.push_back ( connec[11]);   

         edge3.push_back ( connec[8]);  edge3.push_back ( connec[9]); 
         edge3.push_back ( connec[10]);   edge3.push_back ( connec[11]);  

         edge4.push_back ( connec[8]);   edge4.push_back ( connec[4]); edge4.push_back ( connec[0]);   

         edges.push_back ( edge1 );
         edges.push_back ( edge2 );
         edges.push_back ( edge3 );
         edges.push_back ( edge4 );

         order = 3; 
         globdat.nodeICount = 8;

         interConnec1.resize(4);

         inodes0.push_back ( connec[0] );
         inodes0.push_back ( connec[3] );
         inodes0.push_back ( connec[11] );
         inodes0.push_back ( connec[8] );
         inodes0.push_back ( inodes0[0] );

         break;

       case 10: //5x2 elements 

         edge1.push_back ( connec[0]);   edge1.push_back ( connec[1]);
         edge1.push_back ( connec[2]);   edge1.push_back ( connec[3]); edge1.push_back ( connec[4]);   

         edge2.push_back ( connec[4]);   edge2.push_back ( connec[9]);

         edge3.push_back ( connec[5]);   edge3.push_back ( connec[6]);
         edge3.push_back ( connec[7]);   edge3.push_back ( connec[8]); edge3.push_back ( connec[9]);   
         
         
         edge4.push_back ( connec[5]);   edge4.push_back ( connec[0]);

         edges.push_back ( edge1 );
         edges.push_back ( edge2 );
         edges.push_back ( edge3 );
         edges.push_back ( edge4 );

         order = 4; 
         globdat.nodeICount = 10;

         interConnec1.resize(5);

         inodes0.push_back ( connec[0] );
         inodes0.push_back ( connec[4] );
         inodes0.push_back ( connec[9] );
         inodes0.push_back ( connec[5] );
         inodes0.push_back ( inodes0[0] );

         break;

       case 15: //5x3 elements 

         edge1.push_back ( connec[0]);   edge1.push_back ( connec[1]);
         edge1.push_back ( connec[2]);   edge1.push_back ( connec[3]); edge1.push_back ( connec[4]);   

         edge2.push_back ( connec[4]);   edge2.push_back ( connec[9]); edge2.push_back ( connec[14]);

         edge3.push_back ( connec[10]);   edge3.push_back ( connec[11]);
         edge3.push_back ( connec[12]);   edge3.push_back ( connec[13]); edge3.push_back ( connec[14]);   
         
         
         edge4.push_back ( connec[10]);   edge4.push_back ( connec[5]); edge4.push_back ( connec[0]);

         edges.push_back ( edge1 );
         edges.push_back ( edge2 );
         edges.push_back ( edge3 );
         edges.push_back ( edge4 );

         order = 4; 
         globdat.nodeICount = 10;

         interConnec1.resize(5);

         inodes0.push_back ( connec[0] );
         inodes0.push_back ( connec[4] );
         inodes0.push_back ( connec[14] );
         inodes0.push_back ( connec[10] );
         inodes0.push_back ( inodes0[0] );

         break;
     }

     nnode = inodes0.size(); 

     // loop over edges, add interface along common edge

     for ( int in = 0; in < nnode-1; in++ )
     {
       n1 = inodes0[in];
       n2 = inodes0[in+1];

       o1 = globdat.nodeId2Position[n1];
       o2 = globdat.nodeId2Position[n2];

       m1 = globdat.nodeSet[o1]->getDuplicity ();
       m2 = globdat.nodeSet[o2]->getDuplicity ();

       // not a common edge, omits

       if ( ( m1 == 0 ) || ( m2 == 0 ) ) continue;
       if ( ( m1 == 1 ) || ( m2 == 1 ) ) continue;

       // edge on external boundary, also omitted

       if ( find ( globdat.nodePairs.begin(), 
                          globdat.nodePairs.end  (), 
                          NodePair(n1,n2) ) != globdat.nodePairs.end() )
       {
         continue;
       }

       // ignore edge already added

       if ( find ( doneEdges.begin(), 
                          doneEdges.end  (), 
                          NodePair(n1,n2) ) != doneEdges.end() )
       {
         break;
       }

       // existing notch

       if ( globdat.isNotch )
       {
         bool val1, val2;

         double x1 = globdat.nodeSet[o1]->getX ();
         double y1 = globdat.nodeSet[o1]->getY ();

         double x2 = globdat.nodeSet[o2]->getX ();
         double y2 = globdat.nodeSet[o2]->getY ();

         for ( int is = 0; is < globdat.segment.size(); is++ )
         {
           val1 = globdat.segment[is].isOn ( x1, y1 );
           val2 = globdat.segment[is].isOn ( x2, y2 );

           if ( val1 && val2 ) break;
         }

         if ( val1 && val2 ) break; // do not add interface on existing notch
       }

       interConnec1 = edges[in];
       
       // there are exceptional cases edge = n1 n2 n3 
       // where n1, n3 are on material interface but n2 isn't.
       // In this case this edge is not a material interface
       // and therefore no interface element is added here. 

       int ss = interConnec1.size();

       if ( ss > 2 )
       {
         o1 = globdat.nodeId2Position[interConnec1[1]];
         m1 = globdat.nodeSet[o1]->getDuplicity ();
         if ( m1 == 0 || m1 == 1 ) continue;
       }

       //print(edges[in].begin(),edges[in].end());
       //cout << m1 << " " << m2 << "\n"; 
      
       // nodes of upper face

       //if ( ep->getChanged() )
       //{
       //  for ( unsigned ii = ss; ii-- > 0 ; )
       //  {
       //    interConnec.push_back (globdat.duplicatedNodes0[interConnec1[ii]][0]) ;
       //  }
       //  
       //  for ( unsigned ii = ss; ii-- > 0 ; )
       //  {
       //    interConnec.push_back (globdat.duplicatedNodes0[interConnec1[ii]][1]) ;
       //  }
       //}
       //else
       //{
         for (int ii = 0; ii < ss; ii++)
         {
           interConnec.push_back (globdat.duplicatedNodes0[interConnec1[ii]][0]) ;
         }
         
         for (int ii = 0; ii < ss; ii++)
         {
           interConnec.push_back (globdat.duplicatedNodes0[interConnec1[ii]][1]) ;
         }
       //}

       globdat.interfaceSet.push_back ( ElemPointer ( new Element ( ieCount, interConnec ) ) );
       
       globdat.interfaceMats.push_back (0);
       ieCount++;

       doneEdges.push_back ( NodePair(n1,n2) );
    }

    edge1.clear();
    edge2.clear();
    edge3.clear();
    edge4.clear();
    edges.clear();
    interConnec.clear();
    inodes0.clear();
  }
}

// ------------------------------------------------------------------
//    addInterface
// ------------------------------------------------------------------

// add interface element (linear or quadratic) connecting nodes
// n1 [m12]   n2
// p1 [m120]  p2

void                     addInterface

         ( IntVector& interConnec,
           int        n1,
           int        n2,
           int        m12,
           int        p1,
           int        p2,
           bool       changed,
           Global&    globdat )
{
  if ( !globdat.isQuadratic )
  {
    if ( !changed )
    {
      interConnec[0] = n1; interConnec[1] = n2 ;
      interConnec[2] = p1; interConnec[3] = p2 ;
    }
    else
    {
      //interConnec[0] = n2; interConnec[1] = n1;
      interConnec[2] = p1; interConnec[3] = p2;
    }
  }
  else
  {
    if ( !changed )
    {
      interConnec[0] = n1;
      interConnec[1] = globdat.duplicatedNodes0[m12][0]; // midside node
      interConnec[2] = n2;

      interConnec[3] = p1;
      interConnec[4] = globdat.duplicatedNodes0[m12][1]; // midside node
      interConnec[5] = p2;
    }
    else
    {
      //interConnec[0] = n2;
      interConnec[1] = globdat.duplicatedNodes0[m12][0]; 
      //interConnec[2] = n1;

      interConnec[3] = p2;
      interConnec[4] = globdat.duplicatedNodes0[m12][1]; 
      interConnec[5] = p1;
    }
  }
}

// ----------------------------------------------------------
//    findCommonEdge
// ----------------------------------------------------------

void                     findCommonEdge

         ( int&               p1,
           int&               p2,
           int                n1,
           int                n2,
           int                ielem,
           const IntVector&   neighbors,
           Global&            globdat )

{
  IntVector                 jnodes, jnodes0;
  IntVector::const_iterator it1, it2;

  ElemPointer               jp;

  int                       jelem;

  const int                 neiCount = neighbors.size ();

  // loop over neighbors of element ie

  for ( int je = 0; je < neiCount; je++ )
  {
     jelem = globdat.elemId2Position[neighbors[je]];
     jp    = globdat.elemSet[jelem];

     // omit element being considered

     if ( jp->getIndex() == ielem ) 
     {
       continue; 
     }

     jp->getCornerConnectivity  ( jnodes  );
     jp->getCornerConnectivity0 ( jnodes0 );

     it1 = find ( jnodes0.begin(), jnodes0.end(), n1 );
     it2 = find ( jnodes0.begin(), jnodes0.end(), n2 );

     // common edge between ielem and jelem found

     if ( ( it1 != jnodes0.end() ) && 
          ( it2 != jnodes0.end() ) )
     {
       p1 = jnodes[it1 - jnodes0.begin()];
       p2 = jnodes[it2 - jnodes0.begin()];

       break; // only have ONE edge in common
     }
  }
}

