
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

#ifndef JEM_NUMERIC_SPARSE_SPARSEMATRIX_H
#define JEM_NUMERIC_SPARSE_SPARSEMATRIX_H

#include <jem/numeric/sparse/SparseStructure.h>


JEM_BEGIN_PACKAGE( numeric )


//-----------------------------------------------------------------------
//   class SparseMatrix
//-----------------------------------------------------------------------


template <class T>

  class SparseMatrix

{
 public:

  typedef SparseStructure   Structure;
  typedef Structure::Shape  Shape;


                            SparseMatrix      ();

  explicit                  SparseMatrix

    ( const Structure&        s );

                            SparseMatrix

    ( const Structure&        s,
      const Array<T>&         values );

                            SparseMatrix

    ( const Shape&            sh,
      const Array<idx_t>&     offsets,
      const Array<idx_t>&     indices );

                            SparseMatrix

    ( const Shape&            sh,
      const Array<idx_t>&     offsets,
      const Array<idx_t>&     indices,
      const Array<T>&         values );

                            SparseMatrix

    ( const SparseMatrix&     rhs );

  template <class Input>

    void                    readFrom

    ( Input&                  in  );

  template <class Output>

    void                    writeTo

    ( Output&                 out )              const;

  SparseMatrix&             operator =

    ( const SparseMatrix&     rhs );

  void                      swap

    ( SparseMatrix&           rhs );

  SparseMatrix              clone             () const;
  SparseMatrix              transpose         () const;
  inline bool               isValid           () const;
  inline bool               isSorted          () const;
  inline bool               isContiguous      () const;
  inline Structure          getStructure      () const;
  inline idx_t              nonZeroCount      () const;
  inline Shape              shape             () const;

  inline idx_t              size

    ( int                     idim )             const;

  inline
    const Array<idx_t>&     getRowOffsets     () const;
  inline
    const Array<idx_t>&     getColumnIndices  () const;

  inline Array<idx_t>       getColumnIndices

    ( idx_t                   irow )             const;

  inline const Array<T>&    getValues         () const;

  inline Array<T>           getValues

    ( idx_t                   irow )             const;

  inline const idx_t*       getOffsetPtr      () const;
  inline const idx_t*       getIndexPtr       () const;
  inline T*                 getValuePtr       () const;


 private:

  Structure                 struct_;
  Array<T>                  values_;

};


//-----------------------------------------------------------------------
//   related functions
//-----------------------------------------------------------------------


template <class T>

  inline void               swap

  ( SparseMatrix<T>&          lhs,
    SparseMatrix<T>&          rhs );

template <class Input, class T>

  inline void               decode

  ( Input&                    in,
    SparseMatrix<T>&          a );

template <class Output, class T>

  inline void               encode

  ( Output&                   out,
    const SparseMatrix<T>&    a );

template <class Output, class T>

  void                      print

  ( Output&                   out,
    const SparseMatrix<T>&    a );




//#######################################################################
//   Implementation
//#######################################################################

JEM_END_PACKAGE( numeric )

#include <jem/numeric/sparse/SparseMatrix.tcc>

#endif
