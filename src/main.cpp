//
// DVS main program driver.
//

#include <iostream>

#include "dvs.h"

int main( int argc_, char **argv_ )
{

  DVS dvs;

  Error err = dvs.ParseCommands( argc_, argv_ );

  if ( !err.empty( ) )
  {
    std::cerr << err << std::endl;
  }

  int retStatus = !err.empty( );
  return retStatus;
}
