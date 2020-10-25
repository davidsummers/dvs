#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <sys/stat.h>

#include "docopt.h"

#include "dvs.h"

#include "CommandParser.h"
#include "Hex.h"
#include "Keccak.h"

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
    err = Init( );
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

  if ( docopt::value hashOption = args_[ "hash" ];
       hashOption && hashOption.isBool( ) && hashOption.asBool( ) )
  {
    docopt::value stdinOption = args_[ "-i" ];
    bool stdInput = stdinOption && stdinOption.isBool( ) && stdinOption.asBool( );
    docopt::value writeOption = args_[ "-w" ];
    bool write = writeOption && writeOption.isBool( ) && writeOption.asBool( );
    if ( stdInput )
    {
      err = Hash( std::cin, write );
    }
    else
    {
      if ( docopt::value fileOption = args_[ "<file>" ];
           fileOption && fileOption.isString( ) && !fileOption.asString( ).empty( ) )
      {
        std::ifstream inputFile( fileOption.asString( ), std::ios_base::binary );
        if ( inputFile.is_open( ) )
        {
          err = Hash( inputFile, write );
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


std::string DVS::Init( )
{
  std::cout << "Initializing..." << std::endl;

  std::filesystem::path rootPath = DVS_DIR;

  // Create initial control directory structure
  if ( std::filesystem::exists( rootPath ) &&
       std::filesystem::is_directory( rootPath ) )
  {
    std::stringstream ss;
    ss << "Directory " << DVS_DIR << " already exists.";
    return ss.str( );
  }

  // Create DVS directory.
  if ( !std::filesystem::create_directory( rootPath ) )
  {
    std::stringstream ss;
    ss << "Can't create directory '" << rootPath << "'";
    return ss.str( );
  }

  // Create DVS/objects directory.
  std::filesystem::path objectsDir = rootPath;
  objectsDir /= "objects";

  if ( !std::filesystem::create_directory( objectsDir ) )
  {
    std::stringstream ss;
    ss << "Can't create directory '" << objectsDir << "'";
    return ss.str( );
  }

  std::string validate_error;
  validate_error = Validate( );

  if ( !validate_error.empty( ) )
  {
    std::stringstream ss;
    ss << "Can't validate " << DVS_DIR << " directory: " + validate_error;
    return ss.str( );
  }

  std::cout << "Initialized empty DVS repository in " << std::filesystem::absolute( rootPath ) << std::endl;

  return ""; // No error.
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


std::string DVS::Hash( std::istream &str_, const bool write_ )
{
  if ( std::string validate_error = Validate( );
       !validate_error.empty( ) )
  {
    std::stringstream ss;
    ss << "Can't validate " << DVS_DIR << " directory: " + validate_error;
    return ss.str( );
  }

  const int SHA_BUF_SIZE = 4096;
  Keccak hashObj( 256 );
  uint8_t buffer[ SHA_BUF_SIZE ];

  do
  {
    memset( &buffer[ 0 ], 0, sizeof( buffer ) );
    str_.read( reinterpret_cast< char * >( &buffer[ 0 ] ), sizeof( buffer ) );
    if ( std::streamsize bytesRead = str_.gcount( );
         bytesRead > 0 )
    {
      hashObj.addData( &buffer[ 0 ], 0, static_cast< unsigned int>( bytesRead ) );
    }
  } while ( str_.good( ) );

  str_.clear( );

	std::ostringstream hashSs;

	std::vector< unsigned char > op = hashObj.digest();

	for ( auto &oi : op )
	{
		Hex( oi, [ &hashSs ] ( unsigned char a_ )
    {
      hashSs << a_;
    } );
	}

	std::cout << hashSs.str() << std::endl;

  if ( write_ )
  {
    str_.seekg( std::ios_base::beg );

    std::filesystem::path objectPath = DVS_DIR;
    objectPath /= "objects";
    objectPath /= hashSs.str( ).substr( 0, 2 );
    std::string shardedHash = hashSs.str( ).substr( 2 );

    std::filesystem::create_directories( objectPath );

    objectPath /= shardedHash;

    if ( std::filesystem::exists( objectPath ) )
    {
      std::stringstream ss;
      ss << "Error: File '" << objectPath << "' already exists.";
      return ss.str( );
    }

    std::ofstream outputFile( objectPath,  std::ios_base::binary );

    while ( outputFile.good( ) )
    {
      memset( &buffer[ 0 ], 0, sizeof( buffer ) );
      str_.read( reinterpret_cast< char * >( &buffer[ 0 ] ), sizeof( buffer ) );
      if ( std::streamsize bytesRead = str_.gcount( );
           bytesRead > 0 )
      {
        outputFile.write( reinterpret_cast< const char * >( &buffer[ 0 ] ), bytesRead );
      }
      else
      {
        break;
      }
    }
  }

  return "";
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
