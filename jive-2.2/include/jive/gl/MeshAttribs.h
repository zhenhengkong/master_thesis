
/*
 *  Copyright (C) 2016 DRG. All rights reserved.
 *
 *  This file is part of Jive, an object oriented toolkit for solving
 *  partial differential equations.
 *
 *  Commercial License Usage
 *
 *  This file may be used under the terms of a commercial license
 *  provided with the software, or under the terms contained in a written
 *  agreement between you and DRG. For more information contact DRG at
 *  http://www.dynaflow.com.
 *
 *  GNU Lesser General Public License Usage
 *
 *  Alternatively, this file may be used under the terms of the GNU
 *  Lesser General Public License version 2.1 or version 3 as published
 *  by the Free Software Foundation and appearing in the file
 *  LICENSE.LGPLv21 and LICENSE.LGPLv3 included in the packaging of this
 *  file. Please review the following information to ensure the GNU
 *  Lesser General Public License requirements will be met:
 *  https://www.gnu.org/licenses/lgpl.html and
 *  http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
 *  This file is part of Jive, an object oriented toolkit for
 *  solving partial differential equations.
 *
 *  Jive version: 2.2
 *  Date:         Thu 28 Jan 10:31:15 CET 2016
 */

#ifndef JIVE_GL_MESHATTRIBS_H
#define JIVE_GL_MESHATTRIBS_H

#include <jem/gl/StdAttribs.h>
#include <jive/gl/import.h>


JIVE_BEGIN_PACKAGE( gl )


//-----------------------------------------------------------------------
//   class MeshAttribs
//-----------------------------------------------------------------------


class MeshAttribs : public jem::gl::StdAttribs
{
 public:

  JEM_DECLARE_CLASS       ( MeshAttribs, jem::gl::StdAttribs );

  typedef
    jem::gl::Attributes     Attributes;

  static const int          EDGE_COLOR    = Super::LAST_FIELD << 1;
  static const int          NODE_COLOR    = EDGE_COLOR        << 1;
  static const int          SHOW_EDGES    = NODE_COLOR        << 1;
  static const int          SHOW_LINES    = SHOW_EDGES        << 1;
  static const int          SHOW_FACES    = SHOW_LINES        << 1;

  static const int          LAST_FIELD    = SHOW_FACES;

  static const char*        EDGE_COLOR_NAME;
  static const char*        NODE_COLOR_NAME;
  static const char*        SHOW_EDGES_NAME;
  static const char*        SHOW_LINES_NAME;
  static const char*        SHOW_FACES_NAME;


                            MeshAttribs   ();

                            MeshAttribs

  ( const Self&               rhs );

  virtual Ref<Object>       clone         () const;

  virtual int               copyFrom

    ( const Properties&       props );

  virtual void              copyTo

    ( const Properties&       props,
      int                     fields )       const;

  virtual void              setDefaults   ();

  void                      setOption

    ( int                     what,
      bool                    yesno = true );

  inline bool               testOption

    ( int                     what )         const;

  inline Color              getEdgeColor  () const;

  void                      setEdgeColor

    ( const Color&            color );

  inline Color              getNodeColor  () const;

  void                      setNodeColor

    ( const Color&            color );


 protected:

  virtual                  ~MeshAttribs   ();


 protected:

  int                       options_;
  Color                     edgeColor_;
  Color                     nodeColor_;

};





//#######################################################################
//   Implementation
//#######################################################################

//-----------------------------------------------------------------------
//   testOption
//-----------------------------------------------------------------------


inline bool MeshAttribs::testOption ( int what ) const
{
  return ((options_ & what) != 0);
}


//-----------------------------------------------------------------------
//   getEdgeColor
//-----------------------------------------------------------------------


inline jem::gl::Color MeshAttribs::getEdgeColor () const
{
  return edgeColor_;
}


//-----------------------------------------------------------------------
//   getNodeColor
//-----------------------------------------------------------------------


inline jem::gl::Color MeshAttribs::getNodeColor () const
{
  return nodeColor_;
}


JIVE_END_PACKAGE( gl )

#endif
