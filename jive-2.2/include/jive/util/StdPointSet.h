
/*
 *  Copyright (C) 2016 DRG. All rights reserved.
 *
 *  This file is part of Jive, an object oriented toolkit for solving
 *  partial differential equations.
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
 *  This file is part of Jive, an object oriented toolkit for
 *  solving partial differential equations.
 *
 *  Jive version: 2.2
 *  Date:         Thu 28 Jan 10:31:15 CET 2016
 */

#ifndef JIVE_UTIL_STDPOINTSET_H
#define JIVE_UTIL_STDPOINTSET_H

#include <jem/base/Clonable.h>
#include <jem/io/Serializable.h>
#include <jive/util/XPointSet.h>


JIVE_BEGIN_PACKAGE( util )


class XItemMap;


//-----------------------------------------------------------------------
//   class StdPointSet
//-----------------------------------------------------------------------


class StdPointSet : public XPointSet,
                    public Clonable,
                    public Serializable
{
 public:

  JEM_DECLARE_CLASS       ( StdPointSet, XPointSet );


                            StdPointSet ();

                            StdPointSet

    ( const String&           name,
      const String&           pointName,
      const Ref<XItemMap>&    map = NIL );

                            StdPointSet

    ( const Self&             rhs );

  virtual void              resetEvents   ();

  virtual void              readFrom

    ( ObjectInput&            in  );

  virtual void              writeTo

    ( ObjectOutput&           out )          const;

  virtual Ref<Object>       clone         () const;
  virtual idx_t             size          () const;
  virtual idx_t             rank          () const;
  virtual String            getItemName   () const;
  virtual ItemMap*          getItemMap    () const;
  virtual void              clear         ();

  virtual void              reserve

    ( idx_t                   count );

  virtual void              trimToSize    ();

  virtual idx_t             addPoint

    ( const Vector&           coords );

  virtual idx_t             addPoint

    ( idx_t                   id,
      const Vector&           coords );

  virtual idx_t             addPoints

    ( const Matrix&           coords );

  virtual void              reorderPoints

    ( const Reordering&       reord );

  virtual void              getPointCoords

    ( const Vector&           coords,
      idx_t                   ipoint )       const;

  virtual double            getPointXCoord

    ( idx_t                   ipoint,
      idx_t                   idim )         const;

  virtual void              setPointCoords

    ( idx_t                   ipoint,
      const Vector&           coords );

  virtual void              setPointXCoord

    ( idx_t                   ipoint,
      double                  coord,
      idx_t                   idim );

  virtual void              getCoords

    ( const Matrix&           coords )       const;

  virtual void              getXCoords

    ( const Vector&           coords,
      idx_t                   idim )         const;

  virtual void              getSomeCoords

    ( const Matrix&           coords,
      const IdxVector&        ipoints )      const;

  virtual void              getSomeXCoords

    ( const Vector&           coords,
      const IdxVector&        ipoints,
      idx_t                   idim )         const;

  virtual void              setCoords

    ( const Matrix&           coords );

  virtual void              setXCoords

    ( const Vector&           coords,
      idx_t                   idim );

  virtual void              setSomeCoords

    ( const IdxVector&        ipoints,
      const Matrix&           coords );

  virtual void              setSomeXCoords

    ( const IdxVector&        ipoints,
      const Vector&           coords,
      idx_t                   idim );


 protected:

  virtual                  ~StdPointSet   ();

  virtual void              emitEvents_

    ( int                     events );


 private:

  class                     Data_;

  Ref<Data_>                data_;
  Ref<XItemMap>             map_;

};


JIVE_END_PACKAGE( util )

#endif
