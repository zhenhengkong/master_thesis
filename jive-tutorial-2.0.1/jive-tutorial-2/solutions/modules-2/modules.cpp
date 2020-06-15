
#include <jive/fem/InputModule.h>
#include <jive/app/Application.h>
#include <jive/app/InfoModule.h>
#include <jive/app/ChainModule.h>
#include <jive/app/ControlModule.h>

using namespace jem;
using namespace jive::app;
using namespace jive::fem;

//-----------------------------------------------------------------------
//   mainModule
//-----------------------------------------------------------------------

Ref<Module> mainModule ()
{
  Ref<ChainModule>  chain = newInstance<ChainModule> ();

  chain->pushBack ( newInstance<InputModule>  () );
  chain->pushBack ( newInstance<InfoModule>   () );
  chain->pushBack ( newInstance<ControlModule>() );

  return chain;
}

//-----------------------------------------------------------------------
//   main
//-----------------------------------------------------------------------

int main ( int argc, char** argv )
{
  return Application::exec ( argc, argv, mainModule );
}
