
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

#ifndef JIVE_MP_TABLEEXCHANGER_H
#define JIVE_MP_TABLEEXCHANGER_H

#include <jem/base/Object.h>
#include <jive/util/Table.h>
#include <jive/mp/import.h>


JIVE_BEGIN_PACKAGE( mp )


class BorderSet;


//-----------------------------------------------------------------------
//   class TableExchanger
//-----------------------------------------------------------------------


class TableExchanger : public Object
{
 public:

  JEM_DECLARE_CLASS       ( TableExchanger, Object );


                            TableExchanger

    ( const Ref<Context>&     mpx,
      const BorderSet&        borders );

                            TableExchanger

    ( const Ref<Context>&     mpx,
      const BorderSet&        recvBorders,
      const BorderSet&        sendBorders );

  virtual String            toString        () const;

  void                      exchange

    ( XTable&                 table );

  void                      exchange

    ( XTable&                 table,
      const Vector&           weights );

  ItemSet*                  getItems        () const;

  void                      store

    ( const Properties&       globdat )        const;

  static Ref<Self>          find

    ( const Ref<ItemSet>&     items,
      const Properties&       globdat );

  static Ref<Self>          get

    ( const Ref<ItemSet>&     items,
      const Properties&       globdat );


 protected:

  virtual                  ~TableExchanger  ();


 private:

  void                      init_

    ( const Ref<Context>&     mpx,
      const BorderSet&        recvBorders,
      const BorderSet&        sendBorders );

  void                      update_         ();
  void                      invalidate_     ();


 private:

  class                     XData_;
  class                     RecvData_;
  class                     SendData_;
  class                     Utils_;

  friend class              RecvData_;
  friend class              SendData_;
  friend class              Utils_;

  static const int          XTAG_;

  Ref<RecvData_>            recvData_;
  Ref<SendData_>            sendData_;

  bool                      updated_;

};


JIVE_END_PACKAGE( mp )

#endif
