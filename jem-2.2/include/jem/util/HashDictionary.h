
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

#ifndef JEM_UTIL_HASHDICTIONARY_H
#define JEM_UTIL_HASHDICTIONARY_H

#include <jem/base/atomic.h>
#include <jem/base/Clonable.h>
#include <jem/io/Serializable.h>
#include <jem/util/Dictionary.h>


JEM_BEGIN_PACKAGE( util )


//-----------------------------------------------------------------------
//   class HashDictionary
//-----------------------------------------------------------------------


class HashDictionary : public Dictionary,
                       public Clonable,
                       public io::Serializable
{
 public:

  JEM_DECLARE_CLASS       ( HashDictionary, Dictionary );


                            HashDictionary  ();

  virtual void              readFrom

    ( io::ObjectInput&        in );

  virtual void              writeTo

    ( io::ObjectOutput&       out )            const;

  virtual Ref<Object>       clone           () const;
  virtual Ref<DictEnum>     getDictEnum     () const;

  virtual Ref<Object>       get

    ( const String&           key )            const;

  virtual bool              find

    ( Ref<Object>&            value,
      const String&           key )            const;

  virtual void              insert

    ( const String&           key,
      const Ref<Object>&      value );

  virtual void              erase

    ( const String&           key );

  virtual void              clear           ();

  virtual void              reserve

    ( idx_t                   count );

  virtual void              trimToSize      ();
  virtual idx_t             capacity        () const;
  virtual idx_t             size            () const;


 protected:

  virtual                  ~HashDictionary  ();


 private:

  class                     Item_
  {
   public:

    String                    key;
    lint                      hash;
    Ref<Object>               value;
  };

  class                     Node_;
  friend class              Node_;
  class                     Enumerator_;
  friend class              Enumerator_;


 private:

  Item_*                    find_

    ( const String&           key )              const;

  void                      erase_

    ( Item_*                  item );

  idx_t                     unlink_

    ( Node_*                  node );

  void                      realloc_

    ( idx_t                   cap );

  inline void               checkWritable_    ();

  inline idx_t              getTableIndex_

    ( lint                    hash )             const;

 private:

  static const int          CACHE_SIZE_ = 4;

  idx_t*                    table_;
  Node_*                    nodes_;
  Item_                     cache_ [CACHE_SIZE_];

  idx_t                     capacity_;
  idx_t                     itemCount_;
  int                       tableBits_;

  AtomicInteger             enumCount_;

};


//-----------------------------------------------------------------------
//   related types
//-----------------------------------------------------------------------


typedef HashDictionary      HashDict;


JEM_END_PACKAGE( util )

#endif
