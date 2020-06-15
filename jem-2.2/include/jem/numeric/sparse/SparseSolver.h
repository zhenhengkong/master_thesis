
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

#ifndef JEM_NUMERIC_SPARSE_SPARSESOLVER_H
#define JEM_NUMERIC_SPARSE_SPARSESOLVER_H

#include <jem/base/array/Array.h>


JEM_BEGIN_PACKAGE( numeric )


//-----------------------------------------------------------------------
//   class SparseSolver
//-----------------------------------------------------------------------


class SparseSolver
{
 public:

  typedef SparseSolver    Self;


                          SparseSolver      ();

  void                    setZeroThreshold

    ( double                eps );

  inline double           getZeroThreshold  () const;

  static bool             isValidPerm

    ( const Array<idx_t>&   iperm );


 protected:

  double                  zeroThreshold_;


 private:

                          SparseSolver ( const Self& );
  Self&                   operator =   ( const Self& );

};





//#######################################################################
//   Implementation
//#######################################################################

//-----------------------------------------------------------------------
//   getZeroThreshold
//-----------------------------------------------------------------------


inline double SparseSolver::getZeroThreshold () const
{
  return zeroThreshold_;
}



JEM_END_PACKAGE( numeric )

#endif