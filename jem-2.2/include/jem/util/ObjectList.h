
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

#ifndef JEM_UTIL_OBJECTLIST_H
#define JEM_UTIL_OBJECTLIST_H

#include <jem/base/Clonable.h>
#include <jem/io/Serializable.h>
#include <jem/util/ObjectSequence.h>


JEM_BEGIN_PACKAGE( util )


//-----------------------------------------------------------------------
//   class ObjectList
//-----------------------------------------------------------------------


class ObjectList : public ObjectSequence,
                   public Clonable,
                   public io::Serializable
{
 public:

  JEM_DECLARE_CLASS       ( ObjectList, ObjectSequence );


                            ObjectList      ();

  virtual void              readFrom

    ( io::ObjectInput&        in  );

  virtual void              writeTo

    ( io::ObjectOutput&       out )            const;

  virtual Ref<Object>       clone           () const;
  virtual Ref<SeqEnum>      getSeqEnum      () const;

  virtual void              insert

    ( idx_t                   idx,
      const Ref<Object>&      obj );

  virtual void              insertSpace

    ( idx_t                   idx,
      idx_t                   count );

  virtual void              erase

    ( idx_t                   idx );

  virtual void              eraseRange

    ( idx_t                   from,
      idx_t                   to );

  virtual Ref<Object>       get

    ( idx_t                   idx )            const;

  virtual void              set

    ( idx_t                   idx,
      const Ref<Object>&      obj );

  virtual bool              add

    ( const Ref<Object>&      obj );

  virtual Ref<Object>       front           () const;
  virtual Ref<Object>       back            () const;

  virtual void              pushBack

    ( const Ref<Object>&      obj );

  virtual void              pushFront

    ( const Ref<Object>&      obj );

  virtual void              popBack         ();
  virtual void              popFront        ();
  virtual void              clear           ();

  virtual void              reserve

    ( idx_t                   count );

  virtual void              trimToSize      ();
  virtual idx_t             capacity        () const;
  virtual idx_t             size            () const;


 protected:

  virtual                  ~ObjectList      ();


 private:

  inline idx_t              ipos_

    ( idx_t                   idx )            const;

  inline void               incr_

    ( idx_t&                  ipos )           const;

  inline void               decr_

    ( idx_t&                  ipos )           const;

  void                      realloc_

    ( idx_t                   cap );

  inline bool               isWrapped_      () const;
  inline void               checkWritable_  ();


 private:

  class                     Enumerator_;
  friend class              Enumerator_;

  Ref<Object>*              items_;
  idx_t                     head_;
  idx_t                     size_;
  idx_t                     capacity_;
  AtomicInteger             enumCount_;

};


//-----------------------------------------------------------------------
//   related types
//-----------------------------------------------------------------------


typedef ObjectList          ObjList;


JEM_END_PACKAGE( util )

#endif
