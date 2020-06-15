
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

#ifndef JEM_NUMERIC_SPARSE_SPARSESTRUCTURE_H
#define JEM_NUMERIC_SPARSE_SPARSESTRUCTURE_H

#include <jem/base/TypeTraits.h>
#include <jem/base/array/Array.h>


namespace jem
{
  namespace io
  {
    class DataInput;
    class DataOutput;
    class TextOutput;
  }
}


JEM_BEGIN_PACKAGE( numeric )


//-----------------------------------------------------------------------
//   class SparseStructure
//-----------------------------------------------------------------------


class SparseStructure
{
 public:

  typedef SparseStructure   Self;
  typedef Tuple<idx_t,2>      Shape;


                            SparseStructure ();

                            SparseStructure

    ( const Shape&            sh,
      const Array<idx_t>&     offsets,
      const Array<idx_t>&     indices );

                            SparseStructure

    ( const Self&             rhs );

  void                      readFrom

    ( io::DataInput&          in  );

  void                      writeTo

    ( io::DataOutput&         out )              const;

  Self&                     operator =

    ( const Self&             rhs );

  void                      swap

    ( Self&                   rhs );

  Self                      clone             () const;
  Self                      transpose         () const;
  bool                      isValid           () const;
  bool                      isSorted          () const;
  bool                      isSymmetric       () const;
  inline bool               isContiguous      () const;
  inline idx_t              nonZeroCount      () const;
  inline Shape              shape             () const;

  inline idx_t              size

    ( int                     idim )             const;

  inline
    const Array<idx_t>&     getRowOffsets     () const;
  inline
    const Array<idx_t>&     getColumnIndices  () const;

  inline  Array<idx_t>      getColumnIndices

    ( idx_t                   irow )             const;

  inline const idx_t*       getOffsetPtr      () const;
  inline const idx_t*       getIndexPtr       () const;


 protected:

  Shape                     shape_;
  Array<idx_t>              offsets_;
  Array<idx_t>              indices_;

};


typedef SparseStructure     SparseStruct;


//-----------------------------------------------------------------------
//   related functions
//-----------------------------------------------------------------------


inline void                 swap

  ( SparseStructure&          lhs,
    SparseStructure&          rhs );

SparseStructure             matmul

  ( const SparseStructure&    a,
    const SparseStructure&    b );

void                        sortRows

  ( SparseStructure&          s );

void                        findEqualRows

  ( const Array<idx_t>&       rmap,
    const SparseStructure&    s );

void                        findSimilarRows

  ( const Array<idx_t>&       rmap,
    const SparseStructure&    s );

template <class Input>

  inline void               decode

  ( Input&                    in,
    SparseStructure&          s );

template <class Output>

  inline void               encode

  ( Output&                   out,
    const SparseStructure&    s );

void                        print

  ( io::TextOutput&           out,
    const SparseStructure&    s );




//#######################################################################
//   Implementation
//#######################################################################

//=======================================================================
//   class SparseStructure
//=======================================================================

//-----------------------------------------------------------------------
//   isContiguous
//-----------------------------------------------------------------------


inline bool SparseStructure::isContiguous () const
{
  return (offsets_.stride() + indices_.stride() == JEM_IDX_C(2));
}


//-----------------------------------------------------------------------
//   nonZeroCount
//-----------------------------------------------------------------------


inline idx_t SparseStructure::nonZeroCount () const
{
  return indices_.size ();
}


//-----------------------------------------------------------------------
//   shape
//-----------------------------------------------------------------------


inline SparseStructure::Shape SparseStructure::shape () const
{
  return shape_;
}


//-----------------------------------------------------------------------
//   size
//-----------------------------------------------------------------------


inline idx_t SparseStructure::size ( int idim ) const
{
  return shape_[idim];
}


//-----------------------------------------------------------------------
//   getRowOffsets
//-----------------------------------------------------------------------


inline const Array<idx_t>&

  SparseStructure::getRowOffsets () const

{
  return offsets_;
}


//-----------------------------------------------------------------------
//   getColumnIndices
//-----------------------------------------------------------------------


inline const Array<idx_t>&

  SparseStructure::getColumnIndices () const

{
  return indices_;
}


inline Array<idx_t>

  SparseStructure::getColumnIndices ( idx_t irow ) const

{
  return indices_[slice(offsets_[irow],offsets_[irow + 1])];
}


//-----------------------------------------------------------------------
//   getOffsetPtr
//-----------------------------------------------------------------------


inline const idx_t* SparseStructure::getOffsetPtr () const
{
  return offsets_.addr ();
}


//-----------------------------------------------------------------------
//   getIndexPtr
//-----------------------------------------------------------------------


inline const idx_t* SparseStructure::getIndexPtr () const
{
  return indices_.addr ();
}


//=======================================================================
//   related functions
//=======================================================================

//-----------------------------------------------------------------------
//   swap
//-----------------------------------------------------------------------


inline void                 swap

  ( SparseStructure&          lhs,
    SparseStructure&          rhs )

{
  lhs.swap ( rhs );
}


//-----------------------------------------------------------------------
//   decode
//-----------------------------------------------------------------------


template <class Input>

  inline void               decode

  ( Input&                    in,
    SparseStructure&          s )

{
  s.readFrom ( in );
}


//-----------------------------------------------------------------------
//   encode
//-----------------------------------------------------------------------


template <class Output>

  inline void               encode

  ( Output&                   out,
    const SparseStructure&    s )

{
  s.writeTo ( out );
}


JEM_END_PACKAGE( numeric )

JEM_BEGIN_PACKAGE_BASE


//=======================================================================
//   class TypeTraits< numeric::SparseStructure >
//=======================================================================


template <>

  class TypeTraits< numeric::SparseStructure > :

    public BasicTypeTraits

{
 public:

  typedef numeric::
    SparseStructure     SparseStruct;

  static const bool     IS_SERIALIZABLE = true;


  static String         whatis    ()
  {
    return String ( "a sparse matrix structure" );
  }


  static String         typeName  ()
  {
    return String ( "jem::numeric::SparseStructure" );
  }


  static SparseStruct   clone     ( const SparseStruct& rhs )
  {
    return rhs.clone ();
  }

};


JEM_END_PACKAGE_BASE

#endif
