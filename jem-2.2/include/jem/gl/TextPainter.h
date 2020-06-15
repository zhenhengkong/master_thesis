
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

#ifndef JEM_GL_TEXTPAINTER_H
#define JEM_GL_TEXTPAINTER_H

#include <jem/gl/Font.h>
#include <jem/gl/Size.h>
#include <jem/gl/Alignment.h>


JEM_BEGIN_PACKAGE( gl )


//-----------------------------------------------------------------------
//   class TextPainter
//-----------------------------------------------------------------------


class TextPainter
{
 public:

  explicit                  TextPainter

    ( const Font&             font   = Font (),
      Alignment               hAlign = LEFT_ALIGNED,
      Alignment               vAlign = BOT_ALIGNED );

  inline void               begin

    ( GfxContext&             gtx );

  inline void               end         ();

  GLsizei                   write

    ( char                    ch );

  inline GLsizei            writeLine

    ( const char*             str );

  inline GLsizei            writeLine

    ( const String&           str );

  GLsizei                   writeLine

    ( const char*             str,
      idx_t                   len );

  Size2i                    writeBlock

    ( const String&           text );

  void                      writeBlock

    ( const String&           text,
      const Size2i&           size );

  Size2i                    getSizeOf

    ( const String&           text )       const;

  inline void               setCursor

    ( GLint                   x,
      GLint                   y );

  inline void               setCursor

    ( GLint                   x,
      GLint                   y,
      GLfloat                 z );


 public:

  Font                      font;
  GLint                     xPos;
  GLint                     yPos;
  GLfloat                   zPos;
  GLfloat                   angle;
  Alignment                 hAlignment;
  Alignment                 vAlignment;


 private:

                            TextPainter

    ( const TextPainter&      rhs );

  TextPainter&              operator =

    ( const TextPainter&      rhs );

  inline void               rotate_     ();


 private:

  static const GLfloat      HALIGN_[3];
  static const GLfloat      VALIGN_[3];
  static const GLfloat      LINE_SKIP_;

};


//-----------------------------------------------------------------------
//   class TextScope
//-----------------------------------------------------------------------


class TextScope : private FontScope
{
 public:

  explicit inline           TextScope

    ( GfxContext&             gtx,
      TextPainter&            txp );

};




//#######################################################################
//   Implementation
//#######################################################################

//=======================================================================
//   class TextPainter
//=======================================================================

//-----------------------------------------------------------------------
//   begin
//-----------------------------------------------------------------------


inline void TextPainter::begin ( GfxContext& gtx )
{
  font.begin ( gtx );
}


//-----------------------------------------------------------------------
//   end
//-----------------------------------------------------------------------


inline void TextPainter::end ()
{
  font.end ();
}


//-----------------------------------------------------------------------
//   writeLine
//-----------------------------------------------------------------------


inline GLsizei TextPainter::writeLine ( const char* str )
{
  return writeLine ( str, String::sizeOf( str ) );
}


inline GLsizei TextPainter::writeLine ( const String& str )
{
  return writeLine ( str.addr(), str.size() );
}


//-----------------------------------------------------------------------
//   setCursor
//-----------------------------------------------------------------------


inline void TextPainter::setCursor

  ( GLint  x,
    GLint  y )

{
  xPos = x;
  yPos = y;
  zPos = 0.0F;
}


inline void TextPainter::setCursor

  ( GLint    x,
    GLint    y,
    GLfloat  z )

{
  xPos = x;
  yPos = y;
  zPos = z;
}


//=======================================================================
//   class TextScope
//=======================================================================

//-----------------------------------------------------------------------
//   constructor & destructor
//-----------------------------------------------------------------------


inline TextScope::TextScope

  ( GfxContext&   gtx,
    TextPainter&  txp ) :

    FontScope ( gtx, txp.font )

{}


JEM_END_PACKAGE( gl )

#endif
