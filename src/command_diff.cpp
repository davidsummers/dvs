#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>

#include "command_diff.h"
#include "dvs.h"

Error DiffCommand::ParseArgs( DocOptArgs &args_ )
{
  Error err;

  return err;
}

Error DiffCommand::operator( )( DVS &dvs_ )
{
  if ( Error validateError = dvs_.Validate( ); !validateError.empty( ) )
  {
    return validateError;
  }

  Error err = Diff( dvs_ );

  return err;
}

Error DiffCommand::Diff( DVS &dvs_ )
{
  Error err;

  err = "DIFF COMMAND NOT IMPLEMENTED YET.";

  return err;
}
