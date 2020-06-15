
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

#ifndef JEM_UTIL_RANDOM_H
#define JEM_UTIL_RANDOM_H

#include <jem/base/TypeTraits.h>


namespace jem
{
  namespace io
  {
    class DataInput;
    class DataOutput;
  }
}


JEM_BEGIN_PACKAGE( util )


//-----------------------------------------------------------------------
//   class Random
//-----------------------------------------------------------------------


class Random
{
 public:

  explicit                Random

    ( int                   seed = 0 );

                         ~Random        ();

  void                    restart

    ( int                   seed = 0 );

  double                  next          ();

  idx_t                   next

    ( idx_t                 ubound );

  double                  nextGaussian  ();

  void                    readFrom

    ( io::DataInput&        in  );

  void                    writeTo

    ( io::DataOutput&       out )          const;


 private:

                          Random

    ( const Random&         rhs );

  Random&                 operator =

    ( const Random&         rhs );


 private:

  static const int        COEFFC_;
  static const int        LARGE_;
  static const int        SEED_;

  int*                    coeffs_;
  int                     ipos_;
  int                     jpos_;
  int                     igauss_;
  double                  xgauss_;

};


//-----------------------------------------------------------------------
//   Related functions
//-----------------------------------------------------------------------


template <class Input>

  inline void             decode

  ( Input&                  in,
    Random&                 rand );

template <class Output>

  inline void             encode

  ( Output&                 out,
    const Random&           rand );



//#######################################################################
//  Implementation
//#######################################################################

//-----------------------------------------------------------------------
//   decode
//-----------------------------------------------------------------------


template <class Input>

  inline void             decode

  ( Input&                  in,
    Random&                 rand )

{
  rand.readFrom ( in );
}


//-----------------------------------------------------------------------
//   encode
//-----------------------------------------------------------------------


template <class Output>

  inline void             encode

  ( Output&                 out,
    const Random&           rand )

{
  rand.writeTo ( out );
}


JEM_END_PACKAGE( util )

JEM_BEGIN_PACKAGE_BASE


//=======================================================================
//   class TypeTraits<util::Random>
//=======================================================================


template <>

  class TypeTraits<util::Random> : public BasicTypeTraits

{
 public:

  static const bool IS_SERIALIZABLE  = true;
  static const bool HAS_TRIVIAL_COPY = false;
  static const bool HAS_TRIVIAL_DTOR = false;


  static String     whatis   ()
  {
    return String ( "a random number generator" );
  }

  static String     typeName ()
  {
    return String ( "jem::util::Random" );
  }

};


JEM_END_PACKAGE_BASE

#endif
