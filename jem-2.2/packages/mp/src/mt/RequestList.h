
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

#ifndef JEM_MP_MT_REQUESTLIST_H
#define JEM_MP_MT_REQUESTLIST_H

#include <jem/util/Flex.h>
#include <jem/mp/RequestList.h>


JEM_BEGIN_PACKAGE   ( mp )
JEM_BEGIN_NAMESPACE ( mt )


class Arena;
class Request;


//-----------------------------------------------------------------------
//   class RequestList
//-----------------------------------------------------------------------


class RequestList : public mp::RequestList
{
 public:

  typedef RequestList       Self;
  typedef mp::RequestList   Super;


                            RequestList

    ( int                     rank,
      const Ref<Arena>&       arena );

  virtual idx_t             size        () const;

  virtual void              clear

    ( Status*                 stats );

  virtual idx_t             addRequest

    ( const SendBuffer&       buf,
      int                     dest,
      int                     tag );

  virtual idx_t             addRequest

    ( const RecvBuffer&       buf,
      int                     src,
      int                     tag );

  virtual void              startOne

    ( idx_t                   ireq );

  virtual void              startAll    ();

  virtual bool              testOne

    ( idx_t                   ireq,
      Status*                 stat );

  virtual void              testSome

    ( idx_t&                  count,
      idx_t*                  ireqs,
      Status*                 stats );

  virtual bool              testAll

    ( Status*                 stats );

  virtual void              waitOne

    ( idx_t                   ireq,
      Status*                 stat );

  virtual void              waitSome

    ( idx_t&                  count,
      idx_t*                  ireqs,
      Status*                 stats );

  virtual void              waitAll

    ( Status*                 stats );

  virtual void              cancelOne

    ( idx_t                   ireq );

  virtual void              cancelAll

    ( Status*                 stats );


 protected:

  virtual                  ~RequestList ();


 private:

  class                     Utils_;
  friend class              Utils_;


  const int                 rank_;
  Ref<Arena>                arena_;

  util::Flex
    < Ref<Request> >        reqs_;

  util::Flex<idx_t>         iperm_;
  util::Flex<idx_t>         jperm_;
  util::Flex<idx_t>         waitQueue_;

  idx_t                     activeCount_;

};


JEM_END_NAMESPACE ( mt )
JEM_END_PACKAGE   ( mp )

#endif
