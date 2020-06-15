
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

#ifndef JIVE_GEOM_SIMPLEBOXMANAGER_H
#define JIVE_GEOM_SIMPLEBOXMANAGER_H

#include <jem/io/Serializable.h>
#include <jive/geom/import.h>
#include <jive/geom/BoxManager.h>


JIVE_BEGIN_PACKAGE( geom )


//-----------------------------------------------------------------------
//   class SimpleBoxManager
//-----------------------------------------------------------------------


class SimpleBoxManager : public BoxManager,
                         public Serializable
{
 public:

  JEM_DECLARE_CLASS     ( SimpleBoxManager, BoxManager );


                          SimpleBoxManager ();

  explicit                SimpleBoxManager

    ( idx_t                 rank );

  virtual void            readFrom

    ( ObjectInput&          in  );

  virtual void            writeTo

    ( ObjectOutput&         out )             const;

  virtual idx_t           size             () const;
  virtual idx_t           rank             () const;
  virtual void            clear            ();

  virtual void            reserve

    ( idx_t                 boxCount );

  virtual void            trimToSize       ();
  virtual void            flushChanges     ();

  virtual void            setMaskMatrix

    ( Ref<MaskMatrix>       mask );

  virtual void            getEnclosingBox

    ( const Vector&         box )             const;

  virtual idx_t           addBox

    ( const Vector&         box,
      idx_t                 color );

  virtual idx_t           addBoxes

    ( const Matrix&         boxes,
      idx_t                 color );

  virtual void            reorderBoxes

    ( const Reordering&     reord );

  virtual void            getBox

    ( const Vector&         box,
      idx_t                 ibox )            const;

  virtual void            getBoxes

    ( const Matrix&         boxes )           const;

  virtual void            getSomeBoxes

    ( const Matrix&         boxes,
      const IdxVector&      iboxes )          const;

  virtual void            setColors

    ( const IdxVector&      colors );

  virtual void            setSomeColors

    ( const IdxVector&      iboxes,
      const IdxVector&      colors );

  virtual void            updateBoxes

    ( const Matrix&         boxes );

  virtual void            updateSomeBoxes

    ( const IdxVector&      iboxes,
      const Matrix&         boxes );

  virtual idx_t           findChangedBoxes

    ( const IdxVector&      iboxes,
      const Matrix&         boxes )           const;

  virtual idx_t           findBoxNeighbors

    ( const IdxVector&      iboxes,
      idx_t                 jbox )            const;

  virtual idx_t           findBoxNeighbors

    ( const IdxVector&      iboxes,
      const Vector&         box,
      idx_t                 color )           const;


 protected:

  virtual                ~SimpleBoxManager ();


 private:

  class                   Data_;

  Ref<Data_>              data_;

};


JIVE_END_PACKAGE( geom )

#endif
