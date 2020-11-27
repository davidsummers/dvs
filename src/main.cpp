//
// DVS main program driver.
//

#include <iostream>

#include "dvs.h"

int main( int argc_, char **argv_ )
{

  DVS dvs;

  DVS::ParseResult result = dvs.ParseArgs( argc_, argv_ );

  if ( !result.errMsg.empty( ) )
  {
    std::cerr << result.errMsg << std::endl;
  }

  int retStatus = !result.errMsg.empty( );
  return retStatus;
}
