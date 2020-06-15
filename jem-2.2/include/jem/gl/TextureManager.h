
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

#ifndef JEM_GL_TEXTUREMANAGER_H
#define JEM_GL_TEXTUREMANAGER_H

#include <jem/base/Object.h>
#include <jem/base/WeakRef.h>
#include <jem/gl/gl.h>


JEM_BEGIN_PACKAGE( gl )


//-----------------------------------------------------------------------
//   class TextureManager
//-----------------------------------------------------------------------


class TextureManager
{
 public:

  typedef TextureManager    Self;


                            TextureManager  ();
                           ~TextureManager  ();

  inline GLuint             getTexture

    ( const void*             addr )           const;

  GLuint                    newTexture

    ( const void*             addr,
      const Ref<Object>&      owner = NIL );

  void                      delTexture

    ( const void*             addr );

  void                      delTextures     ();
  inline bool               hasTextures     () const;


 private:

  void                      realloc_

    ( idx_t                   newCap );

  GLuint                    getTexture_

    ( const void*             addr )           const;


 private:

                            TextureManager

    ( const Self&             rhs );

  Self&                     operator =

    ( const Self&             rhs );


 private:

  struct                    Tex_
  {
    const void*               addr;
    GLuint                    texID;
    WeakRef<Object>           owner;
    bool                      owned;
  };

  Tex_*                     cache_;
  Tex_*                     textures_;
  idx_t                     size_;
  idx_t                     capacity_;

};


//#######################################################################
//   Implementation
//#######################################################################

//-----------------------------------------------------------------------
//   getTexture
//-----------------------------------------------------------------------


inline GLuint TextureManager::getTexture ( const void* addr ) const
{
  if ( cache_->addr == addr )
  {
    return cache_->texID;
  }
  else
  {
    return getTexture_ ( addr );
  }
}


//-----------------------------------------------------------------------
//   hasTextures
//-----------------------------------------------------------------------


inline bool TextureManager::hasTextures () const
{
  return (size_ > 1);
}


JEM_END_PACKAGE( gl )

#endif
