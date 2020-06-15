
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


#include <jem/base/assert.h>
#include <jem/base/System.h>
#include <jem/base/ClassTemplate.h>
#include <jem/gl/gl.h>
#include <jem/gl/byte.h>
#include <jem/gl/GfxContext.h>
#include <jem/gl/TextureException.h>
#include <jem/gl/ColorMap.h>


JEM_DEFINE_CLASS( jem::gl::ColorMap );


JEM_BEGIN_PACKAGE( gl )


//=======================================================================
//   class ColorMap::Texture_
//=======================================================================

// Dummy class that is used for texture ownership.

class ColorMap::Texture_ : public Object
{
 public:

  inline                    Texture_  ();

};


//-----------------------------------------------------------------------
//   constructor
//-----------------------------------------------------------------------


inline ColorMap::Texture_::Texture_ ()
{}


//=======================================================================
//   class ColorMap
//=======================================================================

//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


ColorMap::ColorMap () :

  colors_ ( 3, 2 )

{
  colors_ = 128.0F;
  smooth_ = false;
}


ColorMap::ColorMap ( const GLfloatMatrix& colors ) :

  colors_ ( colors )

{
  JEM_PRECHECK ( colors.size(0) == 3 || colors.size(0) == 4 );

  smooth_  = false;
  enabled_ = false;
}


ColorMap::~ColorMap ()
{}


//-----------------------------------------------------------------------
//   setColors
//-----------------------------------------------------------------------


void ColorMap::setColors ( const GLfloatMatrix& colors )
{
  JEM_PRECHECK ( colors.size(0) == 3 || colors.size(0) == 4 );

  colors_.ref ( colors );

  texture_ = NIL;
}


//-----------------------------------------------------------------------
//   getColor
//-----------------------------------------------------------------------


Color ColorMap::getColor ( GLfloat idx ) const
{
  const idx_t  n = colors_.size (1);

  Color        c;

  if ( n > 0 )
  {
    idx_t  j = (idx_t) (idx * (GLfloat) (n - 1) + 0.5F);

    if      ( j < 0 )
    {
      j = 0;
    }
    else if ( j >= n )
    {
      j = n - 1;
    }

    c[0] = colors_(0,j);
    c[1] = colors_(1,j);
    c[2] = colors_(2,j);

    if ( colors_.size(0) == 4 )
    {
      c[3] = colors_(3,j);
    }
  }

  return c;
}


//-----------------------------------------------------------------------
//   getMinAlpha
//-----------------------------------------------------------------------


GLfloat ColorMap::getMinAlpha () const
{
  GLfloat  minAlpha = 1.0F;

  if ( colors_.size(0) == 4 )
  {
    const idx_t  n = colors_.size (1);

    for ( idx_t j = 0; j < n; j++ )
    {
      if ( colors_(3,j) < minAlpha )
      {
        minAlpha = colors_(3,j);
      }
    }
  }

  return minAlpha;
}


//-----------------------------------------------------------------------
//   makeSmooth
//-----------------------------------------------------------------------


void ColorMap::makeSmooth ( bool choice )
{
  if ( smooth_ != choice )
  {
    smooth_  = choice;
    texture_ = NIL;
  }
}


//-----------------------------------------------------------------------
//   enable_
//-----------------------------------------------------------------------


void ColorMap::enable_ ( GfxContext& gtx )
{
  if ( texture_ == NIL )
  {
    texture_ = newInstance<Texture_> ();
  }

  GLuint  texID = gtx.getTexture ( texture_.get() );

  if ( ! texID )
  {
    try
    {
      texID = initTexture_ ( gtx );
    }
    catch ( const TextureException& ex )
    {
      texID = 0;

      print ( System::warn(),
              "Failed to create a color map: ", ex.what(), '\n' );
    }
  }

  if ( texID )
  {
    glEnable      ( GL_TEXTURE_1D );
    glBindTexture ( GL_TEXTURE_1D, texID );
    glColor3f     ( 1.0F, 1.0F, 1.0F );

    enabled_ = true;
  }
}


//-----------------------------------------------------------------------
//   disable_
//-----------------------------------------------------------------------


void ColorMap::disable_ ()
{
  glDisable     ( GL_TEXTURE_1D );
  glBindTexture ( GL_TEXTURE_1D, 0 );

  enabled_ = false;
}


//-----------------------------------------------------------------------
//   initTexture_
//-----------------------------------------------------------------------


GLuint ColorMap::initTexture_ ( GfxContext& gtx )
{
  JEM_PRECHECK ( texture_ != NIL );

  GLubyteMatrix  tex     = getTexImage_   ( colors_ );
  GLsizei        width   = (GLsizei) tex.size (1);
  void*          texAddr = texture_.get   ();
  GLuint         texID   = gtx.newTexture ( texAddr, texture_ );

  GLenum         filter;
  GLenum         err;
  GLint          w;


  glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );
  glBindTexture ( GL_TEXTURE_1D, texID );

  // Check whether texture can be accomodated.

  glTexImage1D  ( GL_PROXY_TEXTURE_1D, 0, GL_RGBA, width, 0,
                  GL_RGBA, GL_UNSIGNED_BYTE, NULL );

  glGetTexLevelParameteriv ( GL_PROXY_TEXTURE_1D, 0,
                             GL_TEXTURE_WIDTH, &w );

  if ( w == 0 )
  {
    gtx.delTexture ( texAddr );

    throw TextureException ( "not enough texture memory" );
  }

  // Make sure that the texture transformation matrix is sane.

  glMatrixMode    ( GL_TEXTURE );
  glLoadIdentity  ();
  glMatrixMode    ( GL_MODELVIEW );

  // Define the texture.

  if ( smooth_ )
  {
    filter = GL_LINEAR;
  }
  else
  {
    filter = GL_NEAREST;
  }

  glTexParameteri ( GL_TEXTURE_1D,
                    GL_TEXTURE_WRAP_S,     GL_CLAMP );
  glTexParameteri ( GL_TEXTURE_1D,
                    GL_TEXTURE_MAG_FILTER, (GLint) filter );
  glTexParameteri ( GL_TEXTURE_1D,
                    GL_TEXTURE_MIN_FILTER, (GLint) filter );
  glTexEnvi       ( GL_TEXTURE_ENV,
                    GL_TEXTURE_ENV_MODE,   GL_MODULATE );

  glTexImage1D    ( GL_TEXTURE_1D, 0, GL_RGBA, width, 0,
                    GL_RGBA, GL_UNSIGNED_BYTE, tex.addr() );

  if ( (err = glGetError()) != GL_NO_ERROR )
  {
    gtx.delTexture ( texAddr );

    throw TextureException ( err );
  }

  return texID;
}


//-----------------------------------------------------------------------
//   getTexImage_
//-----------------------------------------------------------------------


GLubyteMatrix ColorMap::getTexImage_

  ( const GLfloatMatrix&  colors )

{
  const int      m = (int) colors.size (0);
  const idx_t    n =       colors.size (1);

  GLubyteMatrix  tex;

  double         dx, xj;
  double         a,  b, c;
  int            i, j, k;
  idx_t          jj;


  for ( k = 2; k < n && k < 1024; k = k << 1 ) ;

  tex.resize ( 4, k );

  if ( m == 4 && n == k )
  {
    convert ( tex, colors );

    return tex;
  }

  tex(3,ALL) = (GLubyte) 255;

  if      ( n == 0 )
  {
    tex(slice(BEGIN,3),ALL) = (GLubyte) 128;
  }
  else if ( n == 1 )
  {
    for ( j = 0; j < k; j++ )
    {
      for ( i = 0; i < m; i++ )
      {
        tex(i,j) = toUbyte ( colors(i,0) );
      }
    }
  }
  else
  {
    dx = 1.0 / (k - 1.0);

    for ( j = 0; j < k; j++ )
    {
      xj = dx * (double) (j * (n - 1));
      jj = (idx_t) xj;

      if      ( jj < 0 )
      {
        jj = 0;
      }
      else if ( jj > (n - 2) )
      {
        jj = n - 2;
      }

      a = (double) (jj + 1) - xj;
      b = xj - (double) jj;

      for ( i = 0; i < m; i++ )
      {
        c        = a * colors(i,jj) + b * colors(i,jj + 1);
        tex(i,j) = toUbyte ( c );
      }
    }
  }

  return tex;
}


JEM_END_PACKAGE( gl )
