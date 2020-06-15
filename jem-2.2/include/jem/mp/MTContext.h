
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

#ifndef JEM_MP_MTCONTEXT_H
#define JEM_MP_MTCONTEXT_H

#include <jem/mp/Context.h>


JEM_BEGIN_PACKAGE( mp )


namespace mt
{
  class Arena;
  class ReduceBuffer;
}


//-----------------------------------------------------------------------
//   class MTContext
//-----------------------------------------------------------------------


class MTContext : public Context
{
 public:

  typedef MTContext           Self;
  typedef Context             Super;


  explicit                    MTContext

    ( int                       n );

                              MTContext

    ( int                       rank,
      const Self&               rhs );

                              MTContext

    ( int                       rank,
      const Ref<mt::Arena>&     arena );

  virtual String              getErrorString

    ( int                       err )           const;

  virtual int                 size           () const;
  virtual int                 myRank         () const;
  virtual bool                isShared       () const;

  virtual void                abort

    ( int                       err );

  virtual Ref<Context>        clone ();

  virtual Ref<RequestList>    newRequestList ();

  virtual void                send

    ( const SendBuffer&         buf,
      int                       dest,
      int                       tag );

  virtual void                recv

    ( const RecvBuffer&         buf,
      int                       src,
      int                       tag,
      Status*                   stat );

  virtual Ref<Request>        initSend

    ( const SendBuffer&         buf,
      int                       dest,
      int                       tag );

  virtual Ref<Request>        initRecv

    ( const RecvBuffer&         buf,
      int                       src,
      int                       tag );

  virtual void                barrier        ();

  virtual void                broadcast

    ( const SendBuffer&         buf );

  virtual void                broadcast

    ( const RecvBuffer&         buf,
      int                       root );

  virtual void                reduce

    ( const RecvBuffer&         in,
      const SendBuffer&         out,
      int                       root,
      Opcode                    opcode );

  virtual void                allreduce

    ( const RecvBuffer&         in,
      const SendBuffer&         out,
      Opcode                    opcode );


 protected:

  virtual                    ~MTContext      ();


 private:

  void                        broadcast_

    ( const mt::ReduceBuffer&   buf,
      int                       root );

  void                        initStack_     ();
  inline bool                 checkStack_    () const;
  static void                 initOnce_      ();


 private:

  Ref<mt::Arena>              arena_;
  int                         myRank_;
  void**                      rstack_;
  int                         rdepth_;

};


JEM_END_PACKAGE( mp )

#endif
