#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <sys/stat.h>

#include "docopt.h"

#include "dvs.h"

#include "command_hash.h"
#include "command_init.h"
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
      dvs internal cat <hash>
      dvs internal hash [ -i ] [ -w ] [ <file> ]
      dvs (-h | --help)
      dvs --version

    Options:
      -h --help              Show this help information.
      -i                     Take input from stdin.
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
    err = Status( );
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
      if ( docopt::value hashOption = args_[ "<hash>" ];
           hashOption && hashOption.isString( ) && !hashOption.asString( ).empty( ) )
      {
          err = Cat( hashOption.asString( ) );
      }
      else
      {
          std::stringstream ss;
          ss << "Missing hash identifier.";
          return ss.str( );
      }
  }
  else if ( docopt::value hashOption = args_[ "hash" ];
       hashOption && hashOption.isBool( ) && hashOption.asBool( ) )
  {
    HashCommand hashCommand;
    docopt::value stdinOption = args_[ "-i" ];
    bool stdInput = stdinOption && stdinOption.isBool( ) && stdinOption.asBool( );
    docopt::value writeOption = args_[ "-w" ];
    bool write = writeOption && writeOption.isBool( ) && writeOption.asBool( );
    if ( stdInput )
    {
      err = hashCommand( *this, std::cin, write );
    }
    else
    {
      if ( docopt::value fileOption = args_[ "<file>" ];
           fileOption && fileOption.isString( ) && !fileOption.asString( ).empty( ) )
      {
        std::ifstream inputFile( fileOption.asString( ), std::ios_base::binary );
        if ( inputFile.is_open( ) )
        {
          err = hashCommand( *this, inputFile, write );
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

  return err;
}


std::string DVS::Status( )
{
  if ( std::string validate_error = Validate( );
       !validate_error.empty( ) )
  {
    std::stringstream ss;
    ss << "Can't validate " << DVS_DIR << " directory: " + validate_error;
    return ss.str( );
  }

  std::cout << "Status: " << std::endl;
  return ""; // No error.
}


std::string DVS::Cat( const std::string &hash_id_ )
{
  if ( std::string validateError = Validate( );
       !validateError.empty( ) )
  {
    return validateError;
  }

  std::filesystem::path hashPath = m_DvsDirectory / "objects" / hash_id_.substr( 0, 2 ) / hash_id_.substr( 2 );

  if ( !std::filesystem::exists( hashPath ) )
  {
    std::stringstream ss;
    ss << "Hash " << hash_id_ << " does not exist.";
    return ss.str( );
  }

  std::ifstream inputFile( hashPath, std::ios_base::binary );

  if ( !inputFile.is_open( ) )
  {
    std::stringstream ss;
    ss << "Can't open file " << hashPath << ".";
    return ss.str( );
  }

  uint8_t buffer[ 4096 ];

  do
  {
    memset( &buffer[ 0 ], 0, sizeof( buffer ) );
    inputFile.read( reinterpret_cast< char * >( &buffer[ 0 ] ), sizeof( buffer ) );
    if ( std::streamsize bytesRead = inputFile.gcount( );
         bytesRead > 0 )
    {
      std::cout.write( reinterpret_cast< const char * >( &buffer[ 0 ] ), bytesRead );
    }
  } while ( inputFile.good( ) );

  return "";
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
