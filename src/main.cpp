//
// DVS main program driver.
//

#include "dvs.h"



int main( int argc_, char **argv_ )
{

  DVS dvs;

  int err = dvs.ParseCommands( argc_, argv_ );

  return err;
}
