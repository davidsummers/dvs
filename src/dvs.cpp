#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <sys/stat.h>

#include "docopt.h"

#include "dvs.h"

#include "command_cat.h"
#include "command_hash.h"
#include "command_init.h"
#include "command_read_tree.h"
#include "command_status.h"
#include "command_write_tree.h"
#include "CommandParser.h"


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
      dvs internal cat [ -t ] <hash>
      dvs internal hash <file>
      dvs internal read-tree <hash>
      dvs internal write-tree
      dvs (-h | --help)
      dvs --version

    Options:
      -h --help              Show this help information.
      -i                     Take input from stdin.
      -t                     Type of hash content (blob, commit, tag, tree).
      -w                     Write output to .dvs/object hash store.
      --version              Show version.
)";


DVS::DVS( )
{
  // Save current working directory.
  m_OriginalDirectory = std::filesystem::current_path( );
}


DVS::~DVS( )
{
  // Restore current working directory.
  std::filesystem::current_path( m_OriginalDirectory );
}


int DVS::ParseCommands( int argc_, char **argv_ )
{
  std::map< std::string, docopt::value > args =
    docopt::docopt( USAGE,
                    { argv_ + 1, argv_ + argc_ },
                    true, // Show help if requested.
                    "dvs Version 1.0" // Version string.
                  );

  std::string err;

  if ( docopt::value initOption = args[ "init" ];
       initOption && initOption.isBool( ) && initOption.asBool( ) )
  {
    InitCommand initCommand;
    err = initCommand( *this );
  }
  else if ( docopt::value statusOption = args[ "status" ];
            statusOption && statusOption.isBool( ) && statusOption.asBool( ) )
  {
    StatusCommand statusCommand;
    err = statusCommand( *this );
  }
  else if ( docopt::value internalOption = args[ "internal" ];
       internalOption && internalOption.isBool( ) && internalOption.asBool( ) )
  {
    err = ParseInternalCommands( args );
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


std::string DVS::ParseInternalCommands( std::map< std::string, docopt::value > &args_ )
{
  std::string err;

  if ( docopt::value catOption = args_[ "cat" ];
       catOption && catOption.isBool( ) && catOption.asBool( ) )
  {
      CatCommand catCommand;

      err = catCommand.ParseArgs( args_ );

      if ( !err.empty( ) )
      {
        return err;
      }

      err = catCommand( *this );
  }
  else if ( docopt::value readTreeOption = args_[ "read-tree" ];
            readTreeOption && readTreeOption.isBool( ) && readTreeOption.asBool( ) )
  {
    ReadTreeCommand readTreeCommand;

    err = readTreeCommand.ParseArgs( args_ );

    if ( !err.empty( ) )
    {
      return err;
    }

    err = readTreeCommand( *this );
  }
  else if ( docopt::value hashOption = args_[ "hash" ];
            hashOption && hashOption.isBool( ) && hashOption.asBool( ) )
  {
    HashCommand hashCommand;

    err = hashCommand.ParseArgs( args_ );

    if ( !err.empty( ) )
    {
      return err;
    }

    err = hashCommand( *this );
  }
  else if ( docopt::value writeTreeOption = args_[ "write-tree" ];
            writeTreeOption && writeTreeOption.isBool( ) && writeTreeOption.asBool( ) )
 {
   WriteTreeCommand writeTreeCommand;
   err = writeTreeCommand( *this );
 }

  return err;
}


std::string DVS::Validate( )
{
  std::filesystem::path currentPath = m_OriginalDirectory;

  for ( ; !currentPath.empty( ); currentPath = RemoveLastPathElement( currentPath ) )
  {
    std::filesystem::path testPath = currentPath / DVS_DIR;
    if ( std::filesystem::exists( testPath ) )
    {
      m_DvsDirectory = testPath;
      break;
    }
  }

  if ( currentPath.empty( ) )
  {
    std::stringstream ss;
    ss << "Directory " << DVS_DIR << " does not exist.";
    return ss.str( );
  }

  return ""; // No errors.
}


std::filesystem::path DVS::RemoveLastPathElement( const std::filesystem::path &path_ )
{
  int num = NumPathElements( path_ );
  std::filesystem::path newPath;
  std::filesystem::path::iterator itr = path_.begin( );

  for ( int i = 0; i < num - 1; ++i, ++itr )
  {
      newPath /= *itr;
  }

  return newPath;
}


int DVS::NumPathElements( const std::filesystem::path &path_ )
{
    int num = 0;
    for ( std::filesystem::path::iterator itr = path_.begin( );
          itr != path_.end( );
          ++itr )
    {
      num++;
    }

  return num;
}


std::filesystem::path DVS::GetDvsDirectory( )
{
  return m_DvsDirectory;
}
