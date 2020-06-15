
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

#ifndef JEM_MP_MPI_UTILITIES_H
#define JEM_MP_MPI_UTILITIES_H

#include <jem/mp/mpi.h>
#include <jem/mp/Type.h>
#include <jem/mp/Status.h>


JEM_BEGIN_PACKAGE   ( mp )
JEM_BEGIN_NAMESPACE ( mpi )


//-----------------------------------------------------------------------
//   convertType
//-----------------------------------------------------------------------


extern MPI_Datatype  TYPE_TABLE  [ TYPE_COUNT ];


inline MPI_Datatype  convertType ( Type t )
{
  return TYPE_TABLE[t];
}


//-----------------------------------------------------------------------
//  setStatus
//-----------------------------------------------------------------------


inline void           setStatus

  ( Status&             out,
    const MPI_Status&   in,
    MPI_Datatype        type )

{
  int  size;

  MPI_Get_count ( const_cast<MPI_Status*> ( &in ),
                  type, &size );

  out.source = in.MPI_SOURCE;
  out.tag    = in.MPI_TAG;
  out.error  = in.MPI_ERROR;
  out.size   = (idx_t) size;
}


JEM_END_NAMESPACE ( mpi )
JEM_END_PACKAGE   ( mp )

#endif