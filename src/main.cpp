//
// DVS main program driver.
//

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>

#include "docopt.h"
#include "dvs.h"

const char USAGE [] =
R"(DVS - David's Versioning System.

    Usage:
      dvs checkout
      dvs commit
      dvs fetch
      dvs hash [ -i ] [ -w ] [ <file> ]
      dvs init
      dvs pull
      dvs push
      dvs status
      dvs (-h | --help)
      dvs --version

    Options:
      -h --help              Show this help information.
      -i                     Take input from stdin.
      -w                     Write output to .dvs/object hash store.
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
  if ( docopt::value hashOption = args[ "hash" ];
       hashOption && hashOption.isBool( ) && hashOption.asBool( ) )
  {
    docopt::value stdinOption = args[ "-i" ];
    bool stdInput = stdinOption && stdinOption.isBool( ) && stdinOption.asBool( );
    docopt::value writeOption = args[ "-w" ];
    bool write = writeOption && writeOption.isBool( ) && writeOption.asBool( );
    if ( stdInput )
    {
      err = dvs.Hash( std::cin, write );
    }
    else
    {
      if ( docopt::value fileOption = args[ "<file>" ];
           fileOption && fileOption.isString( ) && !fileOption.asString( ).empty( ) )
      {
        std::ifstream inputFile( fileOption.asString( ), std::ios_base::binary );
        if ( inputFile.is_open( ) )
        {
          err = dvs.Hash( inputFile, write );
        }
        else
        {
          std::stringstream ss;
          ss << "Couldn't open input file '" << fileOption.asString( ) << "'";
          err = ss.str( );
        }
      }
      else
      {
        err = "Missing file name.";
      }
    }
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
