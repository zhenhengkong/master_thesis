#ifndef GLOBAL_H
#define GLOBAL_H

#include "typedefs.h"
#include "utilities.h"

class NodePair;



// ======================================================
//  class Global
// ======================================================


struct Global
{
   NodeSet                  nodeSet;         // set of original nodes
   NodeSet                  newNodeSet;      // set of original nodes+new added nodes
   ElemSet                  elemSet;
   ElemSet                  interfaceSet;

   Int2IntVectMap           nodeSupport; 
   Int2IntVectMap           duplicatedNodes;
   Int2IntVectMap           duplicatedNodes0; // back up for above
   Int2IntVectMap           dom2Elems;
   Int2IntMap               elem2Domain;
   Int2IntMap               nodeId2Position; // given node's id => position in nodeSet
   Int2IntMap               elemId2Position; // given elem's id => position in elemSet
   IntSet                   boundaryNodes;   // id of nodes on the external boundary
   Int2IntSetMap            bndNodesMap;
   vector<IntVector>        elemNeighbors;
   vector<NodePair>         nodePairs;
   IntVector                isolatedNodes; // store physical points from Gmsh
				    // normally on which displacement is applied 

   IntVector                interfaceNodes; // list of nodes on the mat interfaces
   IntVector                interfaceMats;
   vector<IntVector>        interfacePlanes; // for 3D mesh, 
				      // interfacePlanes[i] contains the nodes
   IntVector                oppositeVertices; // used to compute the correct normals 

   vector<NodePair>         ignoredEdges;   // used for polycrystal

   int                      rigidDomain;  // id of domain where no interface element is created.
   int                      nodeElemCount; // no of nodes per solid element
   int                      nodeICount;    // no of nodes per interface element 
				 

   vector<Segment>          segment; // initial notches
   Segment                  ignoredSegment; // no duplicated nodes, interface elements along
				     // this segment even if they are interfacial nodes

   bool			    isEveryWhere;   
   bool			    isInterface ;
   bool			    isDomain    ;
   bool			    isPolycrystal;
   bool			    isNotch      ;
   bool			    isIgSegment ;
   bool			    isQuadratic ;
   bool			    is3D       ;

   bool                     isContinuum; // continuum interface elements or srping elements
   bool                     isNURBS;

                            Global ();
};

#endif
