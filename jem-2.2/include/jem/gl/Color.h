
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

#ifndef JEM_GL_COLOR_H
#define JEM_GL_COLOR_H

#include <jem/base/assert.h>
#include <jem/base/String.h>
#include <jem/gl/gl.h>
#include <jem/gl/import.h>


JEM_BEGIN_PACKAGE( gl )


//-----------------------------------------------------------------------
//   class Color
//-----------------------------------------------------------------------


class Color
{
 public:

  struct                    Def
  {
    const char*               name;
    Color                   (*color) ();
  };

  static const Def          DEFS[];
  static const GLfloat      OPAQUE_ALPHA;


  explicit inline           Color

    ( GLfloat                 c = 0.0 );

  explicit inline           Color

    ( const GLfloat*          c );

  inline                    Color

    ( GLfloat                 r,
      GLfloat                 g,
      GLfloat                 b,
      GLfloat                 a = 1.0 );

  inline                    Color

    ( const Color&            rhs );

  inline Color&             operator =

    ( const Color&            rhs );

  template <class I>
    inline GLfloat&         operator []

    ( I                       i );

  template <class I>
    inline GLfloat          operator []

    ( I                       i )            const;

  inline GLfloat            r             () const;
  inline GLfloat            g             () const;
  inline GLfloat            b             () const;
  inline GLfloat            a             () const;

  inline bool               isOpaque      () const;
  inline       GLfloat*     addr          ();
  inline const GLfloat*     addr          () const;

  static Color              black         ();
  static Color              blue          ();
  static Color              brown         ();
  static Color              cyan          ();
  static Color              gold          ();
  static Color              gray          ();
  static Color              green         ();
  static Color              magenta       ();
  static Color              orange        ();
  static Color              pink          ();
  static Color              purple        ();
  static Color              red           ();
  static Color              shadow        ();
  static Color              silver        ();
  static Color              steel         ();
  static Color              titanium      ();
  static Color              violet        ();
  static Color              white         ();
  static Color              yellow        ();
  static Color              random        ();

  static Color              parse

    ( const String&           str );

  static Color              byName

    ( const String&           name );

  static String             getChoice     ();

  bool                      findName

    ( String&                 name,
      double                  eps = 0.01 )   const;

  String                    toString      () const;

   bool                      configure

    ( const String&           name,
      const Properties&       props );

  void                      getConfig

    ( const String&           name,
      const Properties&       conf )         const;


 private:

  GLfloat                   data_[4];

};


//-----------------------------------------------------------------------
//   related functions
//-----------------------------------------------------------------------


void                        read

  ( io::TextInput&            in,
    Color&                    c );

template <class Output>

  inline void               print

  ( Output&                   out,
    const Color&              c );

bool                        clamp

  ( Color&                    c );

inline Color                clamped

  ( const Color&              c );

inline void                 setCurrent

  ( const Color&              c );

inline void                 getCurrent

  ( Color&                    c );

inline bool                 isOpaque

  ( GLfloat                   alpha );



//#######################################################################
//   Implementation
//#######################################################################

//=======================================================================
//   class Color
//=======================================================================

//-----------------------------------------------------------------------
//   constructors
//-----------------------------------------------------------------------


inline Color::Color ( GLfloat c )
{
  data_[0] = data_[1] = data_[2] = c;
  data_[3] = 1.0;
}


inline Color::Color ( const GLfloat* c )
{
  data_[0] = c[0];
  data_[1] = c[1];
  data_[2] = c[2];
  data_[3] = c[3];
}


inline Color::Color ( GLfloat r, GLfloat g, GLfloat b, GLfloat a )
{
  data_[0] = r;
  data_[1] = g;
  data_[2] = b;
  data_[3] = a;
}


inline Color::Color ( const Color& rhs )
{
  data_[0] = rhs.data_[0];
  data_[1] = rhs.data_[1];
  data_[2] = rhs.data_[2];
  data_[3] = rhs.data_[3];
}


inline Color& Color::operator = ( const Color& rhs )
{
  data_[0] = rhs.data_[0];
  data_[1] = rhs.data_[1];
  data_[2] = rhs.data_[2];
  data_[3] = rhs.data_[3];

  return *this;
}


//-----------------------------------------------------------------------
//   operator []
//-----------------------------------------------------------------------


template <class I>

  inline GLfloat& Color::operator [] ( I i )

{
  JEM_ASSERT ( checkIndex( i, 4 ) );

  return data_[i];
}


template <class I>

  inline GLfloat Color::operator [] ( I i ) const

{
  JEM_ASSERT ( checkIndex( i, 4 ) );

  return data_[i];
}


//-----------------------------------------------------------------------
//   r, g, b, a
//-----------------------------------------------------------------------


inline GLfloat Color::r () const
{
  return data_[0];
}


inline GLfloat Color::g () const
{
  return data_[1];
}


inline GLfloat Color::b () const
{
  return data_[2];
}


inline GLfloat Color::a () const
{
  return data_[3];
}


//-----------------------------------------------------------------------
//   isOpaque
//-----------------------------------------------------------------------


inline bool Color::isOpaque () const
{
  return (data_[3] > OPAQUE_ALPHA);
}


//-----------------------------------------------------------------------
//   addr
//-----------------------------------------------------------------------


inline GLfloat* Color::addr ()
{
  return data_;
}


inline const GLfloat* Color::addr () const
{
  return data_;
}


//=======================================================================
//   related functions
//=======================================================================

//-----------------------------------------------------------------------
//   print
//-----------------------------------------------------------------------


template <class Output>

  inline void     print

  ( Output&         out,
    const Color&    c )

{
  print ( out, c.toString() );
}


//-----------------------------------------------------------------------
//   clamped
//-----------------------------------------------------------------------


inline Color clamped ( const Color& c )
{
  Color  tmp = c;

  clamp ( tmp );

  return tmp;
}


//-----------------------------------------------------------------------
//   setCurrent
//-----------------------------------------------------------------------


inline void setCurrent ( const Color& c )
{
  const GLfloat*  rgba = c.addr ();

  // Note that this function calls glColor3f and glColor4f instead
  // of glColor3fv and glColor4fv because the latter resulted in
  // wrong colors with a specific version of NVIDIA drivers on
  // Windows.

  if ( rgba[3] > Color::OPAQUE_ALPHA )
  {
    ::glColor3f ( rgba[0], rgba[1], rgba[2] );
  }
  else
  {
    ::glColor4f ( rgba[0], rgba[1], rgba[2], rgba[3] );
  }
}


//-----------------------------------------------------------------------
//   getCurrent
//-----------------------------------------------------------------------


inline void getCurrent ( Color& c )
{
  ::glGetFloatv ( GL_CURRENT_COLOR, c.addr() );
}


//-----------------------------------------------------------------------
//   isOpaque
//-----------------------------------------------------------------------


inline bool isOpaque ( GLfloat alpha )
{
  return (alpha > Color::OPAQUE_ALPHA);
}


JEM_END_PACKAGE( gl )

#endif
