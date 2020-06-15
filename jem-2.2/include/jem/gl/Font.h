
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

#ifndef JEM_GL_FONT_H
#define JEM_GL_FONT_H

#include <jem/base/String.h>
#include <jem/gl/import.h>
#include <jem/gl/Size.h>


JEM_BEGIN_PACKAGE( gl )


class GfxContext;


//-----------------------------------------------------------------------
//   enum FontSize
//-----------------------------------------------------------------------


enum FontSize
{
  SMALL_FONT,
  MEDIUM_FONT,
  LARGE_FONT
};


//-----------------------------------------------------------------------
//   enum FontType
//-----------------------------------------------------------------------


enum FontType
{
  MONOSPACE_FONT,
  PROPORTIONAL_FONT,
  SYMBOL_FONT
};


//-----------------------------------------------------------------------
//   FontID
//-----------------------------------------------------------------------


enum FontID
{
  MONOSPACE_SMALL_FONT,
  MONOSPACE_MEDIUM_FONT,
  MONOSPACE_LARGE_FONT,
  PROPORTIONAL_SMALL_FONT,
  PROPORTIONAL_MEDIUM_FONT,
  PROPORTIONAL_LARGE_FONT,
  SYMBOL_SMALL_FONT,
  SYMBOL_MEDIUM_FONT,
  SYMBOL_LARGE_FONT
};


//-----------------------------------------------------------------------
//   class Font
//-----------------------------------------------------------------------


class Font
{
 public:

  static const int          SIZE_COUNT = 3;
  static const int          TYPE_COUNT = 3;


                            Font          ();

  inline                    Font

    ( FontID                  fid );

  inline                    Font

    ( FontType                ftype,
      FontSize                fsize );

  inline                    Font

    ( const Font&             rhs );

  inline Font&              operator =

    ( const Font&             rhs );

  inline Font&              operator =

    ( FontID                  rhs );

  inline bool               operator ==

    ( const Font&             rhs );

  inline bool               operator !=

    ( const Font&             rhs );

  Font&                     operator ++   ();
  Font                      operator ++   (int);
  Font&                     operator --   ();
  Font                      operator --   (int);

  inline void               swap

    ( Font&                   rhs );

  void                      begin

    ( GfxContext&             gtx );

  void                      end           ();

  GLsizei                   write

    ( char                    ch );

  inline GLsizei            write

    ( const char*             str );

  inline GLsizei            write

    ( const String&           str );

  GLsizei                   write

    ( const char*             str,
      idx_t                   len );

  inline FontID             getID         () const;
  inline FontType           getType       () const;
  inline FontSize           getSize       () const;
  GLsizei                   getHeight     () const;

  GLsizei                   getWidthOf

    ( char                    c )            const;

  inline GLsizei            getWidthOf

    ( const char*             str )          const;

  inline GLsizei            getWidthOf

    ( const String&           str )          const;

  GLsizei                   getWidthOf

    ( const char*             str,
      idx_t                   len )          const;

  Size2i                    getSizeOf

    ( char                    c )            const;

  inline Size2i             getSizeOf

    ( const char*             str )          const;

  inline Size2i             getSizeOf

    ( const String&           str )          const;

  Size2i                    getSizeOf

    ( const char*             str,
      idx_t                   len )          const;

  String                    toString      () const;

  bool                      configure

    ( const String&           name,
      const Properties&       props );

  void                      getConfig

    ( const String&           name,
      const Properties&       conf )         const;

  static Font               bestMatch

    ( FontType                ftype,
      GLsizei                 height );

  static Font               getDefault    ();

  static Font               getDefault

    ( FontType                ftype );

  static void               setDefault

    ( const Font&             font );


 private:

  static GLuint             initTexture_

    ( GfxContext&             gtx,
      FontID                  fnID );


 private:

  class                     Data_;

  FontID                    fnID_;

};


//-----------------------------------------------------------------------
//   class FontScope
//-----------------------------------------------------------------------


class FontScope
{
 public:

  explicit inline           FontScope

    ( GfxContext&             gtx,
      const Font&             font );

  inline                   ~FontScope     ();


 private:

  Font                      font_;

};


//-----------------------------------------------------------------------
//   related functions
//-----------------------------------------------------------------------


inline void                 swap

  ( Font&                     lhs,
    Font&                     rhs );

void                        print

  ( TextOutput&               out,
    const Font&               font );



//#######################################################################
//   Implementation
//#######################################################################

//=======================================================================
//   class Font
//=======================================================================

//-----------------------------------------------------------------------
//   constructors
//-----------------------------------------------------------------------


inline Font::Font ( FontID fid )
{
  fnID_ = fid;
}


inline Font::Font

  ( FontType  ftype,
    FontSize  fsize )

{
  fnID_ = (FontID) (ftype * SIZE_COUNT + fsize);
}


inline Font::Font ( const Font& rhs )
{
  fnID_ = rhs.fnID_;
}


//-----------------------------------------------------------------------
//   assignment operators
//-----------------------------------------------------------------------


inline Font& Font::operator = ( const Font& rhs )
{
  fnID_ = rhs.fnID_;

  return *this;
}


inline Font& Font::operator = ( FontID rhs )
{
  fnID_ = rhs;

  return *this;
}


//-----------------------------------------------------------------------
//   comparison operators
//-----------------------------------------------------------------------


inline bool Font::operator == ( const Font& rhs )
{
  return (fnID_ == rhs.fnID_);
}


inline bool Font::operator != ( const Font& rhs )
{
  return (fnID_ != rhs.fnID_);
}


//-----------------------------------------------------------------------
//   swap
//-----------------------------------------------------------------------


inline void Font::swap ( Font& rhs )
{
  FontID  tmpID = fnID_;

  fnID_     = rhs.fnID_;
  rhs.fnID_ = tmpID;
}


//-----------------------------------------------------------------------
//   write
//-----------------------------------------------------------------------


inline GLsizei Font::write ( const char* str )
{
  return write ( str, String::sizeOf( str ) );
}


inline GLsizei Font::write ( const String& str )
{
  return write ( str.addr(), str.size() );
}


//-----------------------------------------------------------------------
//   getID
//-----------------------------------------------------------------------


inline FontID Font::getID () const
{
  return fnID_;
}


//-----------------------------------------------------------------------
//   getType
//-----------------------------------------------------------------------


inline FontType Font::getType () const
{
  return (FontType) (fnID_ / SIZE_COUNT);
}


//-----------------------------------------------------------------------
//   getSize
//-----------------------------------------------------------------------


inline FontSize Font::getSize () const
{
  return (FontSize) (fnID_ % SIZE_COUNT);
}


//-----------------------------------------------------------------------
//   getWidthOf
//-----------------------------------------------------------------------


inline GLsizei Font::getWidthOf ( const char* str ) const
{
  return getWidthOf ( str, String::sizeOf( str ) );
}


inline GLsizei Font::getWidthOf ( const String& str ) const
{
  return getWidthOf ( str.addr(), str.size() );
}


//-----------------------------------------------------------------------
//   getSizeOf
//-----------------------------------------------------------------------


inline Size2i Font::getSizeOf ( const char* str ) const
{
  return getSizeOf ( str, String::sizeOf( str ) );
}


inline Size2i Font::getSizeOf ( const String& str ) const
{
  return getSizeOf ( str.addr(), str.size() );
}


//=======================================================================
//   class FontScope
//=======================================================================

//-----------------------------------------------------------------------
//   constructor & destructor
//-----------------------------------------------------------------------


inline FontScope::FontScope

  ( GfxContext&  gtx,
    const Font&  font ) :

    font_ ( font )

{
  font_.begin ( gtx );
}


inline FontScope::~FontScope ()
{
  font_.end ();
}


//=======================================================================
//   related functions
//=======================================================================

//-----------------------------------------------------------------------
//   swap
//-----------------------------------------------------------------------


inline void   swap

  ( Font&       lhs,
    Font&       rhs )

{
  lhs.swap ( rhs );
}


JEM_END_PACKAGE( gl )

#endif
