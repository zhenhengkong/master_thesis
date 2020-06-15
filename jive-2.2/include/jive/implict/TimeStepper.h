
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

#ifndef JIVE_IMPLICT_TIMESTEPPER_H
#define JIVE_IMPLICT_TIMESTEPPER_H

#include <jem/base/NamedObject.h>
#include <jive/implict/import.h>


JIVE_BEGIN_PACKAGE( implict )


//-----------------------------------------------------------------------
//   class TimeStepper
//-----------------------------------------------------------------------


class TimeStepper : public NamedObject
{
 public:

  JEM_DECLARE_CLASS       ( TimeStepper, NamedObject );

  static const int          NEW_COEFFS;


  explicit                  TimeStepper

    ( const String&           name = "" );

  virtual int               configure

    ( const Properties&       props );

  virtual void              getConfig

    ( const Properties&       conf )         const;

  virtual int               restart

    ( const Properties&       globdat )            = 0;

  virtual int               advance

    ( const Properties&       globdat );

  virtual void              commit

    ( const Properties&       globdat );

  virtual int               cancel

    ( const Properties&       globdat );

  virtual void              updateState

    ( const Properties&       globdat )            = 0;

  virtual void              predictState

    ( const Properties&       globdat )            = 0;

  virtual double            getTimeCoeff

    ( int                     order )        const = 0;

  virtual int               getMaxOrder   () const = 0;
  virtual double            getDeltaTime  () const = 0;

  virtual void              setDeltaTime

    ( double                  dtime )              = 0;

  virtual idx_t             getTimeStep   () const = 0;
  virtual DofSpace*         getDofSpace   () const = 0;
  virtual String            getContext    () const;


 protected:

  virtual                  ~TimeStepper   ();

};


//-----------------------------------------------------------------------
//   related functions
//-----------------------------------------------------------------------


Ref<TimeStepper>            newTimeStepper

  ( const String&             name,
    const Properties&         conf,
    const Properties&         globdat );

Ref<TimeStepper>            newTimeStepper

  ( const String&             name,
    const Properties&         conf,
    const Properties&         props,
    const Properties&         globdat );


JIVE_END_PACKAGE( implict )

#endif
