//
// DVS main program driver.
//

#include <iostream>
#include <map>

#include "docopt.h"
#include "dvs.h"

const char USAGE [] =
R"(DVS - David's Versioning System.

    Usage:
      dvs checkout
      dvs commit
      dvs fetch
      dvs init
      dvs pull
      dvs push
      dvs status
      dvs (-h | --help)
      dvs --version

    Options:
      -h --help              Show this help information.
      --version              Show version.
)";


int main( int argc_, char **argv_ )
{
  std::map< std::string, docopt::value > args =
    docopt::docopt( USAGE,
                    { argv_ + 1, argv_ + argc_ },
                    true, // Show help if requested.
                    "dvs Version 1.0" // Version string.
                  );

  DVS dvs;
  std::string err;

  if ( docopt::value initOption = args[ "init" ];
       initOption && initOption.isBool( ) && initOption.asBool( ) )
  {
    err = dvs.Init( );
  }
  else if ( docopt::value statusOption = args[ "status" ];
            statusOption && statusOption.isBool( ) && statusOption.asBool( ) )
  {
    err = dvs.Status( );
  }
  else
  {
    std::cerr << "Command not yet implemented." << std::endl;
    return 1;
  }

  if ( !err.empty( ) )
  {
    std::cerr << err << std::endl;
    return 1;
  }

  return 0;
}
