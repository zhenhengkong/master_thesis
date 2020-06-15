
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

#ifndef JEM_GL_COLORMAP_H
#define JEM_GL_COLORMAP_H

#include <jem/base/Object.h>
#include <jem/gl/Array.h>
#include <jem/gl/Color.h>


JEM_BEGIN_PACKAGE( gl )


class GfxContext;


//-----------------------------------------------------------------------
//   class ColorMap
//-----------------------------------------------------------------------


class ColorMap : public Object
{
 public:

  JEM_DECLARE_CLASS       ( ColorMap, Object );


                            ColorMap      ();

  explicit                  ColorMap

    ( const GLfloatMatrix&    colors );

  inline void               enable

    ( GfxContext&             gtx );

  inline void               disable       ();

  void                      setColors

    ( const GLfloatMatrix&    colors );

  inline GLfloatMatrix      getColors     () const;

  Color                     getColor

    ( GLfloat                 idx )          const;

  GLfloat                   getMinAlpha   () const;

  void                      makeSmooth

    ( bool                    choice = true );

  inline bool               isSmooth      () const;

  inline void               setColor

    ( GLfloat                 idx )          const;


 protected:

  virtual                  ~ColorMap      ();


 private:

  void                      enable_

    ( GfxContext&             gtx );

  void                      disable_      ();

  GLuint                    initTexture_

    ( GfxContext&             gtx );

  static GLubyteMatrix      getTexImage_

    ( const GLfloatMatrix&    colors );


 private:

  class                     Texture_;

  GLfloatMatrix             colors_;
  Ref<Texture_>             texture_;
  bool                      smooth_;
  bool                      enabled_;

};


//-----------------------------------------------------------------------
//   class ColorMapScope
//-----------------------------------------------------------------------


class ColorMapScope
{
 public:

  explicit inline           ColorMapScope

    ( GfxContext&             gtx,
      ColorMap&               cmap );

  inline                   ~ColorMapScope ();


 private:

  ColorMap&                 cmap_;

};





//#######################################################################
//   Implementation
//#######################################################################

//=======================================================================
//   class ColorMap
//=======================================================================

//-----------------------------------------------------------------------
//   enable
//-----------------------------------------------------------------------


inline void ColorMap::enable ( GfxContext& gtx )
{
  if ( ! enabled_ )
  {
    enable_ ( gtx );
  }
}


//-----------------------------------------------------------------------
//   disable
//-----------------------------------------------------------------------


inline void ColorMap::disable ()
{
  if ( enabled_ )
  {
    disable_ ();
  }
}


//-----------------------------------------------------------------------
//   getColors
//-----------------------------------------------------------------------


inline GLfloatMatrix ColorMap::getColors () const
{
  return colors_;
}


//-----------------------------------------------------------------------
//   isSmooth
//-----------------------------------------------------------------------


inline bool ColorMap::isSmooth () const
{
  return smooth_;
}


//-----------------------------------------------------------------------
//   setColor
//-----------------------------------------------------------------------


inline void ColorMap::setColor ( GLfloat idx ) const
{
  glTexCoord1f ( idx );
}


//=======================================================================
//   class ColorMapScope
//=======================================================================


inline ColorMapScope::ColorMapScope

  ( GfxContext&  gtx,
    ColorMap&    cmap ) :

    cmap_ ( cmap )

{
  cmap_.enable ( gtx );
}


inline ColorMapScope::~ColorMapScope ()
{
  cmap_.disable ();
}


JEM_END_PACKAGE( gl )

#endif
