
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

#ifndef JEM_MP_GATHERSTREAM_H
#define JEM_MP_GATHERSTREAM_H

#include <jem/io/OutputStream.h>
#include <jem/mp/import.h>

JEM_BEGIN_PACKAGE( mp )


class Context;
class InputChannel;
class OutputChannel;


//-----------------------------------------------------------------------
//   class GatherStream
//-----------------------------------------------------------------------


class GatherStream : public OutputStream
{
 public:

  JEM_DECLARE_CLASS       ( GatherStream, OutputStream );

  class                     Session;


                            GatherStream

    ( int                     root,
      const Ref<Context>&     ctx,
      Ref<OutputStream>       out,
      idx_t                   bufsize = -1 );

  static Ref<Self>          open

    ( int                     root,
      const Ref<Context>&     ctx,
      const String&           fname,
      int                     flags   =  0,
      idx_t                   bufsize = -1 );

  virtual void              close         ();
  virtual void              flush         ();

  virtual void              write

    ( const void*             buf,
      idx_t                   n );

  virtual void              writeNoThrow

    ( const void*             buf,
      idx_t                   n );

  void                      beginSession  ();
  void                      endSession    ();


 protected:

  virtual                  ~GatherStream  ();


 private:

  int                       root_;
  int                       myRank_;

  Ref<Context>              context_;
  Ref<OutputStream>         output_;
  Ref<OutputStream>         bitBucket_;
  Ref<OutputChannel>        outChannel_;
  Ref<InputChannel>         inChannel_;

};


//-----------------------------------------------------------------------
//   class GatherStream::Session
//-----------------------------------------------------------------------


class GatherStream::Session
{
 public:

  explicit inline           Session

    ( Ref<GatherStream>      gs );

  inline                   ~Session       ();


 private:

  Ref<GatherStream>         gstream_;

};




//#######################################################################
//   Implementation
//#######################################################################

//=======================================================================
//   class GatherStream::Session
//=======================================================================


inline GatherStream::Session::Session

  ( Ref<GatherStream>  gs ) :

    gstream_ ( gs )

{
  gstream_->beginSession ();
}


inline GatherStream::Session::~Session ()
{
  gstream_->endSession ();
}


JEM_END_PACKAGE( mp )

#endif
