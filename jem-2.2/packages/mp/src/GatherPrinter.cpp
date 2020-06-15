
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
#include <jem/io/FileOutputStream.h>
#include <jem/io/OutputStreamWriter.h>
#include <jem/mp/Context.h>
#include <jem/mp/GatherStream.h>
#include <jem/mp/GatherPrinter.h>


JEM_DEFINE_CLASS( jem::mp::GatherPrinter );


JEM_BEGIN_PACKAGE( mp )


//=======================================================================
//   class GatherPrinter
//=======================================================================

//-----------------------------------------------------------------------
//   constructor & destructor
//-----------------------------------------------------------------------


GatherPrinter::GatherPrinter

  ( Ref<OutputStream>   out,
    idx_t               bufsize ) :

    Super ( newInstance<io::OutputStreamWriter>( out ), bufsize )

{
  gstream_ = dynamicCast<GatherStream> ( out );
}


GatherPrinter::~GatherPrinter ()
{}


//-----------------------------------------------------------------------
//   open
//-----------------------------------------------------------------------


Ref<GatherPrinter> GatherPrinter::open

  ( int                  root,
    const Ref<Context>&  ctx,
    const String&        fname,
    int                  flags,
    idx_t                bufsize )

{
  JEM_PRECHECK ( ctx != NIL );

  if ( ctx->size() == 1 )
  {
    return newInstance<Self> (
      newInstance<io::FileOutputStream> ( fname, flags ),
      bufsize
    );
  }
  else
  {
    return newInstance<Self> (
      GatherStream::open ( root, ctx, fname, flags, bufsize ),
      bufsize
    );
  }
}


Ref<GatherPrinter> GatherPrinter::open

  ( int                  root,
    const Ref<Context>&  ctx,
    Ref<OutputStream>    out,
    idx_t                bufsize )

{
  JEM_PRECHECK ( ctx != NIL );


  if      ( ctx->size() == 1 )
  {
    return newInstance<Self> ( out, bufsize );
  }
  else if ( ctx->myRank() == root )
  {
    return newInstance<Self> (
      newInstance<GatherStream> ( root, ctx, out, bufsize ),
      bufsize
    );
  }
  else
  {
    return newInstance<Self> (
      newInstance<GatherStream> ( root, ctx, NIL, bufsize )
    );
  }
}


//-----------------------------------------------------------------------
//   beginSession
//-----------------------------------------------------------------------


void GatherPrinter::beginSession ()
{
  if ( gstream_ != NIL )
  {
    this    ->flushBuffer  ();
    gstream_->beginSession ();
  }
}


//-----------------------------------------------------------------------
//   endSession
//-----------------------------------------------------------------------


void GatherPrinter::endSession ()
{
  if ( gstream_ != NIL )
  {
    this    ->flushBuffer ();
    gstream_->endSession  ();
  }
}


JEM_END_PACKAGE( mp )
