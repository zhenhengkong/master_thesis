
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

#ifndef JEM_IO_FILTERREADER_H
#define JEM_IO_FILTERREADER_H

#include <jem/io/Reader.h>


JEM_BEGIN_PACKAGE( io )


//-----------------------------------------------------------------------
//   FilterReader
//-----------------------------------------------------------------------


class FilterReader : public Reader
{
 public:

  JEM_DECLARE_CLASS     ( FilterReader, Reader );


  explicit                FilterReader

    ( const Ref<Reader>&    in );

                          FilterReader

    ( const Self&           rhs );

  virtual void            close         ();

  virtual idx_t           poll

    ( const Time&           timeout );

  virtual int             read          ();

  virtual idx_t            read

    ( char*                 buf,
      idx_t                 n );

  virtual idx_t            readUntil

    ( char*                 buf,
      idx_t                 n,
      int                   delim );

  virtual void            pushBack

    ( int                   c );

  virtual void            skipUntil

    ( int                   delim );

  virtual void            skipWhite     ();

  virtual idx_t           skip

    ( idx_t                 n );


 protected:

                          FilterReader  ();
  virtual                ~FilterReader  ();


 protected:

  Ref<Reader>             input_;

};


JEM_END_PACKAGE( io )

#endif
