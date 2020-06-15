
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

#ifndef JEM_UTIL_OBJECTMAP_H
#define JEM_UTIL_OBJECTMAP_H

#include <jem/util/ObjectCollection.h>
#include <jem/util/MapEnumerator.h>


JEM_BEGIN_PACKAGE( util )


//-----------------------------------------------------------------------
//   class ObjectMap
//-----------------------------------------------------------------------


class ObjectMap : public ObjectCollection
{
 public:

  JEM_DECLARE_CLASS       ( ObjectMap, ObjectCollection );


  Ref<MapEnum>              enumerate     () const;
  virtual Ref<Enumerator>   getEnumerator () const;
  virtual Ref<MapEnum>      getMapEnum    () const = 0;

  virtual bool              contains

    ( const Ref<Object>&      key )          const;

  virtual Ref<Object>       get

    ( const Ref<Object>&      key )          const;

  virtual bool              find

    ( Ref<Object>&            value,
      const Ref<Object>&      key )          const = 0;

  virtual void              insert

    ( const Ref<Object>&      key,
      const Ref<Object>&      value )              = 0;

  virtual void              erase

    ( const Ref<Object>&      key )                = 0;

  virtual bool              add

    ( const Ref<Object>&      obj );


 protected:

  virtual                  ~ObjectMap     ();

  void                      clone_

    ( const ObjectMap&        rhs );

  void                      readFrom_

    ( io::ObjectInput&        in );

  void                      writeTo_

    ( io::ObjectOutput&       out )          const;

};


//-----------------------------------------------------------------------
//   related types
//-----------------------------------------------------------------------


typedef ObjectMap           ObjMap;


JEM_END_PACKAGE( util )

#endif
