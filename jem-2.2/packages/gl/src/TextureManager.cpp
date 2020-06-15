
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


#include <jem/base/memory.h>
#include <jem/base/MemCache.h>
#include <jem/gl/TextureException.h>
#include <jem/gl/TextureManager.h>


JEM_BEGIN_PACKAGE( gl )


//=======================================================================
//   class TextureManager
//=======================================================================

/*
  Implementation note: the first entry in the textures_ array is a
  dummy entry that is used for an invalid cache_ pointer. In this way
  the cache_ pointer always points to a valid memory address, speeding
  up the member function getTexture. The cache_ pointer never points
  to a texture with an owner, so that it is not necessary to check
  whether the owner is still alive in the case of a cache hit.
*/

//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


TextureManager::TextureManager ()
{
  size_     = 1;
  capacity_ = 4;
  textures_ = (Tex_*) MemCache::alloc ( (size_t) capacity_ *
                                        sizeof(Tex_) );
  cache_    = textures_;

  // Allocate the cache entry.

  try
  {
    new (cache_) Tex_ ();
  }
  catch ( ... )
  {
    MemCache::dealloc ( textures_, (size_t) capacity_ *
                                   sizeof(Tex_) );
    throw;
  }

  cache_->addr  = NULL;
  cache_->texID = 0;
  cache_->owned = false;
}


TextureManager::~TextureManager ()
{
  // The destructor does *not* free the OpenGL textures as it may be
  // executed outside the correct OpenGL context.

  destroy           ( textures_, size_ );
  MemCache::dealloc ( textures_, (size_t) capacity_ *
                                 sizeof(*textures_) );

  textures_ = cache_ = NULL;
}


//-----------------------------------------------------------------------
//   newTexture
//-----------------------------------------------------------------------


GLuint TextureManager::newTexture

  ( const void*         addr,
    const Ref<Object>&  owner )

{
  delTexture ( addr );

  if ( size_ >= capacity_ )
  {
    realloc_ ( size_ + (size_ / 4) + 1 );
  }

  GLuint  texID;
  GLenum  err;

  // Clear the OpenGL error status and reserve a name for the texture.

  glGetError    ();
  glGenTextures ( 1, &texID );

  if ( (err = glGetError()) != GL_NO_ERROR )
  {
    throw TextureException ( err );
  }

  Tex_*  tex = textures_ + size_;

  new (tex) Tex_ ();

  try
  {
    tex->addr  = addr;
    tex->texID = texID;
    tex->owner = owner;
    tex->owned = (owner != NIL);
  }
  catch ( ... )
  {
    tex->~Tex_       ();
    glDeleteTextures ( 1, &texID );

    throw;
  }

  size_++;

  if ( ! tex->owned )
  {
    cache_ = tex;
  }

  return texID;
}


//-----------------------------------------------------------------------
//   delTexture
//-----------------------------------------------------------------------


void TextureManager::delTexture ( const void* addr )
{
  idx_t  i = 1;

  while ( i < size_ )
  {
    Tex_*  tex = textures_ + i;

    // Delete the specified texture and collect garbage. That is, also
    // delete the textures of which the owner has died.

    if ( tex->addr == addr ||
         (tex->owned && tex->owner.get() == NIL) )
    {
      Tex_*  last = textures_ + (size_ - 1);

      glDeleteTextures ( 1, &(tex->texID) );

      if ( tex != last )
      {
        tex->addr  = last->addr;
        tex->texID = last->texID;
        tex->owned = last->owned;

        tex->owner.swap ( last->owner );
      }

      if ( cache_ == tex )
      {
        cache_ = textures_;
      }

      last->~Tex_ ();

      size_--;
    }
    else
    {
      i++;
    }
  }
}


//-----------------------------------------------------------------------
//   delTextures
//-----------------------------------------------------------------------


void TextureManager::delTextures ()
{
  cache_ = textures_;

  while ( size_ > 1 )
  {
    Tex_*  tex = textures_ + (size_ - 1);

    glDeleteTextures ( 1, &(tex->texID) );
    tex->~Tex_       ();

    size_--;
  }
}


//-----------------------------------------------------------------------
//   realloc_
//-----------------------------------------------------------------------


void TextureManager::realloc_ ( idx_t newCap )
{
  if ( newCap < size_ )
  {
    newCap = size_;
  }

  Tex_*  newTextures =

    (Tex_*) MemCache::alloc ( (size_t) newCap * sizeof(Tex_) );

  try
  {
    construct ( newTextures, size_ );

    for ( idx_t i = 0; i < size_; i++ )
    {
      Tex_*  oldTex = textures_   + i;
      Tex_*  newTex = newTextures + i;

      newTex->addr  = oldTex->addr;
      newTex->texID = oldTex->texID;
      newTex->owned = oldTex->owned;

      newTex->owner.swap ( oldTex->owner );
    }
  }
  catch ( ... )
  {
    MemCache::dealloc ( newTextures, (size_t) newCap * sizeof(Tex_) );
    throw;
  }

  jem::swap ( textures_, newTextures );
  jem::swap ( capacity_, newCap );

  cache_ = textures_;

  destroy           ( newTextures, size_ );
  MemCache::dealloc ( newTextures, (size_t) newCap * sizeof(Tex_) );
}


//-----------------------------------------------------------------------
//   getTexture_
//-----------------------------------------------------------------------


GLuint TextureManager::getTexture_ ( const void* addr ) const
{
  for ( idx_t i = 1; i < size_; i++ )
  {
    Tex_*  tex = textures_ + i;

    if ( tex->addr == addr )
    {
      if      ( ! tex->owned )
      {
        const_cast<Self*> ( this ) -> cache_ = tex;
      }
      else if ( tex->owner.get() == NIL )
      {
        const_cast<Self*> ( this ) -> delTexture ( addr );

        return 0;
      }

      return tex->texID;
    }
  }

  return 0;
}


JEM_END_PACKAGE( gl )
