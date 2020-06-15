
/*
 *  Copyright (C) 2016 DRG. All rights reserved.
 *
 *  This file is part of Jem, a general purpose programming toolkit.
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
 *
 *  Jem version: 2.2
 *  Date:        Thu 28 Jan 10:36:48 CET 2016
 */

#ifndef JEM_GL_TEXTPROJECTOR_H
#define JEM_GL_TEXTPROJECTOR_H

#include <jem/gl/Rect.h>
#include <jem/gl/Transform.h>
#include <jem/gl/TextPainter.h>


JEM_BEGIN_PACKAGE( gl )


//-----------------------------------------------------------------------
//   class TextProjector
//-----------------------------------------------------------------------


class TextProjector : public TextPainter
{
 public:

  typedef TextPainter       Super;


  explicit                  TextProjector

    ( const Font&             font   = Font (),
      Alignment               hAlign = LEFT_ALIGNED,
      Alignment               vAlign = BOT_ALIGNED );

  void                      begin

    ( GfxContext&             gtx );

  void                      begin

    ( GfxContext&             gtx,
      const Transform&        proj );

  void                      end         ();

  inline void               setCursor

    ( GLdouble                x,
      GLdouble                y );

  void                      setCursor

    ( GLdouble                x,
      GLdouble                y,
      GLdouble                z );

  inline bool               isVisible   () const;


 private:

  Transform                 proj_;
  Rect                      vport_;
  bool                      active_;

};


//-----------------------------------------------------------------------
//   class OrthoTextScope
//-----------------------------------------------------------------------


class OrthoTextScope
{
 public:

  explicit inline           OrthoTextScope

    ( GfxContext&             gtx,
      TextProjector&          txp );

  inline                   ~OrthoTextScope  ();


 private:

  TextProjector&            txp_;

};




//#######################################################################
//   Implementation
//#######################################################################

//=======================================================================
//   class TextProjector
//=======================================================================

//-----------------------------------------------------------------------
//   setCursor
//-----------------------------------------------------------------------


inline void TextProjector::setCursor

  ( GLdouble  x,
    GLdouble  y )

{
  setCursor ( x, y, 0.0 );
}


//-----------------------------------------------------------------------
//   isVisible
//-----------------------------------------------------------------------


inline bool TextProjector::isVisible () const
{
  return (vport_.contains( xPos, yPos ) &&
          zPos >= 0.0 && zPos <= 1.0);
}


//=======================================================================
//   class OrthoTextScope
//=======================================================================

//-----------------------------------------------------------------------
//   constructor & destructor
//-----------------------------------------------------------------------


inline OrthoTextScope::OrthoTextScope

  ( GfxContext&     gtx,
    TextProjector&  txp ) :

    txp_ ( txp )

{
  txp_.begin ( gtx );
}


inline OrthoTextScope::~OrthoTextScope ()
{
  txp_.end ();
}


JEM_END_PACKAGE( gl )

#endif
