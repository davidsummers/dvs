//
// DVC main program driver.
//

#include <iostream>

#include "dvs.h"

void usage( );

int main( int argc_, char **argv_ )
{
  if ( argc_ < 2 )
  {
    usage( );
    return 1; // Error.
  }

  DVC dvc;
  std::string command = argv_[1];
  std::string err;

  if ( command == "init" )
  {
    err = dvc.Init( );
  }
  else if ( command == "status" )
  {
    err = dvc.Status( );
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
  std::cerr << "Usage: dvc sub-command" << std::endl;
  std::cerr << "  where sub-command is one of:" << std::endl;
  std::cerr << "    init"     << std::endl;
  std::cerr << "    checkout" << std::endl;
  std::cerr << "    commit"   << std::endl;
  std::cerr << "    fetch"    << std::endl;
  std::cerr << "    pull"     << std::endl;
  std::cerr << "    push"     << std::endl;
  std::cerr << "    status"   << std::endl;
}
