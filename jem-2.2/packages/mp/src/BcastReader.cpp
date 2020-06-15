
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
#include <jem/base/ClassTemplate.h>
#include <jem/io/FileInputStream.h>
#include <jem/mp/Context.h>
#include <jem/mp/BcastStream.h>
#include <jem/mp/BcastReader.h>


using jem::io::InputStream;


JEM_DEFINE_CLASS ( jem::mp::BcastReader );


JEM_BEGIN_PACKAGE( mp )


//=======================================================================
//   class BcastReader
//=======================================================================

//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


BcastReader::BcastReader

  ( int                  root,
    const Ref<Context>&  ctx,
    const String&        fname,
    int                  flags,
    idx_t                bufsize ) :

    Super ( open_( root, ctx, fname, flags, bufsize ) )

{}


BcastReader::BcastReader

  ( int                  root,
    const Ref<Context>&  ctx,
    const Ref<Input>&    in,
    idx_t                bufsize ) :

    Super ( open_( root, ctx, in, bufsize ) )

{}


BcastReader::~BcastReader ()
{}


//-----------------------------------------------------------------------
//   open_
//-----------------------------------------------------------------------


Ref<InputStream> BcastReader::open_

  ( int                  root,
    const Ref<Context>&  ctx,
    const String&        fname,
    int                  flags,
    idx_t                bufsize )

{
  JEM_PRECHECK ( ctx != NIL );

  if ( ctx->size() == 1 )
  {
    return newInstance<io::FileInputStream> ( fname, flags );
  }
  else
  {
    return BcastStream::open ( root, ctx, fname, flags, bufsize );
  }
}


Ref<InputStream> BcastReader::open_

  ( int                  root,
    const Ref<Context>&  ctx,
    const Ref<Input>&    in,
    idx_t                bufsize )

{
  JEM_PRECHECK ( ctx != NIL );

  if      ( ctx->size()   == 1 )
  {
    return in;
  }
  else if ( ctx->myRank() == root )
  {
    return newInstance<BcastStream> ( root, ctx, in,  bufsize );
  }
  else
  {
    return newInstance<BcastStream> ( root, ctx, NIL, bufsize );
  }
}


JEM_END_PACKAGE( mp )