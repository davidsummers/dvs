#include <iostream>
#include <fstream>
#ifdef WIN32
#include <io.h>
#define mktemp _mktemp
#endif

#include "command_hash.h"
#include "dvs.h"
#include "Hex.h"
#include "Keccak.h"


std::string HashCommand::ParseArgs( std::map< std::string, docopt::value > &args_ )
{
    std::string err;

    if ( docopt::value fileOption = args_[ "<file>" ];
          fileOption && fileOption.isString( ) && !fileOption.asString( ).empty( ) )
    {
      m_Filename = fileOption.asString( );
    }
    else
    {
      err = "Missing file name.";
    }

  return err;
}


std::string HashCommand::operator ( ) ( DVS &dvs_ )
{
  if ( std::string validate_error = dvs_.Validate( );
       !validate_error.empty( ) )
  {
    return validate_error;
  }

  auto [ err, oid ] = Hash( dvs_, m_Filename );

  std::cout << oid << std::endl;

  return err;
}

HashCommand::HashResult HashCommand::Hash( DVS &dvs_, const std::string &filename_, const HashType hashType_ )
{
  std::string err;

  std::ostringstream hashSs;

  std::istream *inputStream = nullptr;

  std::ifstream inputFileStream;

  {
    inputFileStream.open( filename_, std::ios_base::binary );

    if ( !inputFileStream.is_open( ) )
    {
      std::stringstream ss;
      ss << "Couldn't open input file '" << filename_ << "'";
      err = ss.str( );
      return { err, "" };
    }

    inputStream = &inputFileStream;
  }

  const int SHA_BUF_SIZE = 4096;
  Keccak hashObj( 256 );
  std::vector< uint8_t > buffer( SHA_BUF_SIZE );
  
  // Add header (hash type) to buffer.
  std::stringstream headerSs;

  switch ( hashType_ )
  {
    case HashType::blob:
      headerSs << "blob " << std::filesystem::file_size( filename_ ) << '\0';
      break;

    case HashType::commit:
      headerSs << "commit\0";
      break;

    case HashType::none:
      return { "HashType: none", "" }; // Illegal - return errror.

    case HashType::tag:
      headerSs << "tag\0";
      break;

    case HashType::tree:
      headerSs << "tree\0";
      break;
  }

  std::string header = headerSs.str( );
  size_t headerSize = header.size( );

  hashObj.addData( reinterpret_cast< uint8_t * >( &header[ 0 ] ), 0, static_cast< unsigned int >( headerSize ) );

  // Read input into buffer and write buffer to hash function.
  do
  {
    memset( &buffer[ 0 ], 0, sizeof( buffer ) );
    inputStream->read( reinterpret_cast< char * >( &buffer[ 0 ] ), sizeof( buffer ) );
    if ( std::streamsize bytesRead = inputStream->gcount( );
          bytesRead > 0 )
    {
      hashObj.addData( &buffer[ 0 ], 0, static_cast< unsigned int>( bytesRead ) );
    }
  } while ( inputStream->good( ) );

  inputStream->clear( );

  std::vector< unsigned char > op = hashObj.digest();

  for ( auto &oi : op )
  {
    Hex( oi, [ &hashSs ] ( unsigned char a_ )
    {
      hashSs << a_;
    } );
  }

  {
    inputStream->seekg( std::ios_base::beg );

    std::filesystem::path objectPath = dvs_.GetDvsDirectory( );
    objectPath /= "objects";
    objectPath /= hashSs.str( ).substr( 0, 2 );
    std::string shardedHash = hashSs.str( ).substr( 2 );

    std::filesystem::create_directories( objectPath );

    objectPath /= shardedHash;

    if ( std::filesystem::exists( objectPath ) )
    {
      std::stringstream ss;
      ss << "Error: File '" << objectPath << "' already exists.";
      return { ss.str( ), "" };
    }

    std::ofstream outputFile( objectPath,  std::ios_base::binary );

    outputFile.write( &header[ 0 ], headerSize );

    while ( outputFile.good( ) )
    {
      memset( &buffer[ 0 ], 0, sizeof( buffer ) );
      inputStream->read( reinterpret_cast< char * >( &buffer[ 0 ] ), sizeof( buffer ) );
      if ( std::streamsize bytesRead = inputStream->gcount( );
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

  return { "", hashSs.str( ) };
}
