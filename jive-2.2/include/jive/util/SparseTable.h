
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

#ifndef JIVE_UTIL_SPARSETABLE_H
#define JIVE_UTIL_SPARSETABLE_H

#include <jem/base/Clonable.h>
#include <jem/io/Serializable.h>
#include <jive/util/XTable.h>


JIVE_BEGIN_PACKAGE( util )


class Reordering;


//-----------------------------------------------------------------------
//   class SparseTable
//-----------------------------------------------------------------------


class SparseTable : public XTable,
                    public Clonable,
                    public Serializable
{
 public:

  JEM_DECLARE_CLASS       ( SparseTable, XTable );


                            SparseTable ();

                            SparseTable

    ( const String&           name,
      const Ref<ItemSet>&     items );

                            SparseTable

    ( const Self&             rhs );

  virtual void              resetEvents     ();

  virtual void              readFrom

    ( ObjectInput&            in  );

  virtual void              writeTo

    ( ObjectOutput&           out )            const;

  virtual Ref<Object>       clone           () const;
  virtual idx_t             size            () const;
  virtual idx_t             columnCount     () const;
  virtual void              clearData       ();
  virtual void              clearAll        ();

  virtual void              reserve

    ( idx_t                   nzCount );

  virtual void              trimToSize      ();

  virtual idx_t             addColumn

    ( const String&           name );

  virtual idx_t             findColumn

    ( const String&           name )           const;

  virtual String            getColumnName

    ( idx_t                   jcol )           const;

  virtual StringVector      getColumnNames  () const;

  virtual void              setData

    ( const idx_t*            irows,
      idx_t                   icount,
      const idx_t*            jcols,
      idx_t                   jcount,
      const double*           values );

  virtual void              addData

    ( const idx_t*            irows,
      idx_t                   icount,
      const idx_t*            jcols,
      idx_t                   jcount,
      const double*           values );

  virtual idx_t             eraseData

    ( const idx_t*            irows,
      idx_t                   icount,
      const idx_t*            jcols,
      idx_t                   jcount );

  virtual idx_t             getData

    ( double*                 buf,
      const idx_t*            irows,
      idx_t                   icount,
      const idx_t*            jcols,
      idx_t                   jcount )         const;

  virtual idx_t             getAllData

    ( double*                 buf,
      idx_t                   jst )            const;

  virtual void              scaleRows

    ( const Vector&           scale );

  virtual SparseMatrix      toMatrix        () const;
  virtual ItemSet*          getRowItems     () const;


 protected:

  virtual                  ~SparseTable     ();

  virtual void              emitEvents_

    ( int                     events );


 private:

  void                      connect_        ();

  void                      itemsResized_

    ( idx_t                   rowCount );

  void                      itemsReordered_

    ( const Reordering&       reord );


 private:

  class                     Data_;

  Ref<Data_>                data_;

};


JIVE_END_PACKAGE( util )

#endif
