
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

#ifndef JIVE_IMPLICT_TRANSIENTMODEL_H
#define JIVE_IMPLICT_TRANSIENTMODEL_H

#include <jem/io/Serializable.h>
#include <jive/Array.h>
#include <jive/model/Model.h>
#include <jive/implict/import.h>


JIVE_BEGIN_PACKAGE( implict )


class TimeStepper;
class TransientMatrix;


//-----------------------------------------------------------------------
//   class TransientModel
//-----------------------------------------------------------------------


class TransientModel : public Model,
                       public Serializable
{
 public:

  JEM_DECLARE_CLASS       ( TransientModel, Model );

  static const char*        TYPE_NAME;
  static const int          MANAGED;


                            TransientModel  ();

                            TransientModel

    ( const String&           name,
      const Ref<Model>&       child,
      Ref<TimeStepper>        stepper,
      int                     options = 0 );

                            TransientModel

    ( const String&           name,
      const Properties&       conf,
      const Properties&       props,
      const Properties&       globdat );

  void                      init

    ( const Properties&       globdat );

  virtual void              readFrom

    ( ObjectInput&            in );

  virtual void              writeTo

    ( ObjectOutput&           out )            const;

  virtual Model*            findModel

    ( const String&           name )           const;

  virtual void              configure

    ( const Properties&       props,
      const Properties&       globdat );

  virtual void              getConfig

    ( const Properties&       conf,
      const Properties&       globdat )        const;

  virtual bool              takeAction

    ( const String&           action,
      const Properties&       params,
      const Properties&       globdat );

  virtual String            getContext      () const;

  void                      setContext

    ( const String&           context );

  static Ref<Model>         makeNew

    ( const String&           name,
      const Properties&       conf,
      const Properties&       props,
      const Properties&       globdat );

  static void               declare         ();


 protected:

  virtual                  ~TransientModel  ();


 private:

  void                      init_           ();

  void                      restart_

    ( const Properties&       globdat );

  void                      advance_

    ( const Properties&       globdat );

  void                      commit_

    ( const Properties&       globdat );

  void                      cancel_

    ( const Properties&       globdat );

  void                      getIntVector_

    ( const Vector&           fint,
      const Properties&       globdat );

  void                      newCoeffs_      ();
  void                      dofsChanged_    ();


 private:

  static const int          STARTED_;
  static const int          ADVANCING_;
  static const int          COMMITTING_;

  String                    context_;

  Ref<Model>                child_;
  Ref<TimeStepper>          stepper_;
  Ref<TransientMatrix>      transmat_;

  Ref<DofSpace>             dofs_;
  Ref<VectorSpace>          vspace_;

  double                    dtMin_;
  double                    dtMax_;
  double                    norm0_;

  int                       status_;
  int                       options_;

  Vector                    vbuf_;

};


JIVE_END_PACKAGE( implict )

#endif
