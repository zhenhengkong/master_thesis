/**
 * This file is a part of the interface element generator program.
 *
 * V.P.Nguyen, TU Delft 2009
 *
 */

#ifndef ELEMENT_H
#define ELEMENT_H

#include "typedefs.h"

class Global;

// =====================================================================
//     class ELEMENT
// =====================================================================

// For quadratic elements:
// the connectivity follows Gmsh's format !!!

class Element
{
  public:

                         Element

	( int index, int elemType, 
	  const IntVector& connec );

                         Element

	( int index, int elemType, 
	  const IntVector& connec, bool isNURBS );

			 Element

	( int index, 
	  const IntVector& connec );

    // get the new connectivity (full)

    inline void		 getConnectivity

        ( IntVector& connec ) const;

    // get the original connectivity (full)
    
    inline void		 getConnectivity0

        ( IntVector& connec ) const;

    // get indices of corner nodes of unmodified mesh
    // for linear elements == getConnectivity0
    // for quadratic elements: 

    void		 getCornerConnectivity0

        ( IntVector& connec ) const;

    void		 getCornerConnectivity

        ( IntVector& connec ) const;

    inline void          setConnectivity

        ( const IntVector& newConnec );

    inline int           getIndex    () const;

    inline int           getElemType () const;

    inline void          setElemType ( int type );

    inline void          setDone ( bool done ); 

    inline bool          getDone    () const;

    inline bool          getChanged () const;

    inline void          changeConnectivity 
      
        ( int oldId, int newId );

    void                 buildFaces ();

    void                 getFaces 
      
         ( vector<IntVector> faces ) const;

    // return all nodes of the face indexed index
    // used for quadratic 3D elements

    inline void          getFullFace 

         ( IntVector& fface,
	   int        index ) const;

   // check if a 3D element has a face on the interface or not
   // if so, return the indices of nodes of that face 
    
   bool                  isOnInterface 
      
         ( IntVector&        face,    
	   int&              oppVertex,
	   int&              fIndex,
	   const NodeSet&    nodeSet,
	         Int2IntMap& position ) const;

   // check if a 3D element has a face on the interface or not 

   bool                  isInterfaceElement 

         ( const NodeSet&    nodeSet,
	         Int2IntMap& position )  const;

   // return the coordinate bounds of a face
   // of a 3D element.

   void                  getBoundsOfFace

         ( pair<double,double>& xBound,
           pair<double,double>& yBound,
           pair<double,double>& zBound,
	   int                  fIndex,
	         Global&        globdat ) const;


   void                  getJemConnectivity

         ( IntVector& connec )            const;

   inline void           setNURBS ( );


  private:

   void                  buildFacesForTet4_  ();
   void                  buildFacesForTet10_ ();
   void                  buildFacesForHex8_  ();
   void                  buildFacesForHex20_ ();


   inline void           getJemConnectLinear_

         ( IntVector& connec )            const; 

   void                  getJemConnect2DQuadratic_

         ( IntVector& connec )            const; 

   void                  getJemConnectTet10_

         ( IntVector& connec )            const; 


   void                  getJemConnectHex20_

         ( IntVector& connec )            const; 

  private:

    int                  index_;
    int                  elemType_;   // used with Paraview format

    IntVector            connectivity_;
    IntVector            connectivity0_;

    bool                 done_;
    bool                 isQuadratic_;
    bool                 isChanged_;
    bool                 isNURBS_;

    // 3D data

    vector<IntVector>    faces_;  // store the corner vertex indices of faces
    vector<IntVector>    ffaces_;  // store the vertex indices of faces
    IntVector            oppositeVertices_; // store the opposite vertices of 
                                            // all faces

    int                  nodePerFace_;
};

// ==========================================================
//   related stuff
// ==========================================================

struct PrintElement;

// ==========================================================
//   implementation of inline functions
// ==========================================================

inline int Element::getIndex () const
{
  return index_;
}

inline int Element::getElemType () const
{
  return elemType_;
}

inline void  Element::setElemType ( int type )
{
  elemType_ = type;
}

inline  void Element::setDone ( bool done ) 
{
  done_ = done;
}

inline bool Element::getDone () const 
{ 
  return done_;
}

inline bool Element::getChanged () const 
{ 
  return isChanged_;
}

inline void Element::getConnectivity

   ( IntVector& connec ) const
{
  connec.resize ( connectivity_.size() );
  connec = connectivity_;	
}

inline void Element::getConnectivity0

   ( IntVector& connec ) const
{
  connec.resize ( connectivity0_.size() );
  connec = connectivity0_;	
}


inline void Element::setConnectivity

        ( const IntVector& newConnec )
{
  connectivity_ = newConnec;
}

inline void Element::changeConnectivity ( int oldId, int newId )
{
  replace ( connectivity_.begin(),
	    connectivity_.end  (),
	    oldId, newId );

  if ( !isChanged_ && newId != oldId )
  {
    isChanged_ = true;
  }
}

inline void Element::getFullFace

   ( IntVector& fface,
     int        index ) const
{
  fface.resize ( ffaces_[0].size() );
  fface = ffaces_[index];	
}

inline void Element::getJemConnectLinear_

   ( IntVector& connec ) const
{
  connec.resize ( connectivity_.size() );
  connec = connectivity_;	
}

inline void Element::setNURBS ()
{
  isNURBS_ = true;
}

#endif
