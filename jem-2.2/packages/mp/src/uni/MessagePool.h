
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

#ifndef JEM_MP_UNI_MESSAGEPOOL_H
#define JEM_MP_UNI_MESSAGEPOOL_H

#include <jem/base/Collectable.h>
#include "Message.h"


JEM_BEGIN_PACKAGE   ( mp )
JEM_BEGIN_NAMESPACE ( uni )


//-----------------------------------------------------------------------
//   class MessagePool
//-----------------------------------------------------------------------


class MessagePool : public Collectable
{
 public:


  inline                MessagePool ();

  void                  queueSend

    ( Message&            msg );

  void                  queueRecv

    ( Message&            msg );


 protected:

  virtual              ~MessagePool ();


 private:

  jem::util::ListNode   rqueue_;
  jem::util::ListNode   squeue_;

};



//#######################################################################
//   Implementation
//#######################################################################

//-----------------------------------------------------------------------
//   constructor
//-----------------------------------------------------------------------


inline MessagePool::MessagePool ()
{}


JEM_END_NAMESPACE ( uni )
JEM_END_PACKAGE   ( mp )

#endif