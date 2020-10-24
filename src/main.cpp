//
// DVS main program driver.
//

#include <iostream>

#include "docopt.h"
#include "dvs.h"

void usage( );

int main( int argc_, char **argv_ )
{
  if ( argc_ < 2 )
  {
    usage( );
    return 1; // Error.
  }

  DVS dvs;
  std::string command = argv_[1];
  std::string err;

  if ( command == "init" )
  {
    err = dvs.Init( );
  }
  else if ( command == "status" )
  {
    err = dvs.Status( );
  }
  else
  {
    std::cerr << "Command '" << command << "' not yet implemented." << std::endl;
    return 1;
  }

  if ( !err.empty( ) )
  {
    std::cerr << err << std::endl;
    return 1;
  }

  return 0;
}

void usage( )
{
  std::cerr << "Usage: dvs sub-command" << std::endl;
  std::cerr << "  where sub-command is one of:" << std::endl;
  std::cerr << "    init"     << std::endl;
  std::cerr << "    checkout" << std::endl;
  std::cerr << "    commit"   << std::endl;
  std::cerr << "    fetch"    << std::endl;
  std::cerr << "    pull"     << std::endl;
  std::cerr << "    push"     << std::endl;
  std::cerr << "    status"   << std::endl;
}
