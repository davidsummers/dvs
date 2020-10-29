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


using HashMapLookup = std::map< HashCommand::HashType, std::string >;

HashMapLookup s_HashMapLookup =
{
  { HashCommand::HashType::none, "none" },
  { HashCommand::HashType::blob, "blob" },
  { HashCommand::HashType::tag,  "tag"  },
  { HashCommand::HashType::tree, "tree" },
};


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


OidResult HashCommand::Hash( DVS &dvs_, const std::string &filename_, const HashType hashType_ )
{
  std::ifstream inputFileStream( filename_, std::ios_base::binary );

  if ( !inputFileStream.is_open( ) )
  {
    std::stringstream ss;
    ss << "Couldn't open input file '" << filename_ << "'";
    std::string err = ss.str( );
    return { err, "" };
  }

  size_t size = std::filesystem::file_size( filename_ );

  auto [ err, oid ] = Hash( dvs_, inputFileStream, size, hashType_ );

  return { err, oid };
}


OidResult HashCommand::Hash( DVS &dvs_, std::istream &inputStream_, size_t size_, HashType hashType_ )
{
  std::ostringstream hashSs;

  const int SHA_BUF_SIZE = 4096;
  Keccak hashObj( 256 );
  std::vector< uint8_t > buffer( SHA_BUF_SIZE );
  
  // Add header (hash type) to buffer.
  std::stringstream headerSs;

  switch ( hashType_ )
  {
    case HashType::blob:
      headerSs << "blob " << size_ << '\0';
      break;

    case HashType::commit:
      headerSs << "commit" << '\0';
      break;

    case HashType::none:
      return { "HashType: none", "" }; // Illegal - return errror.

    case HashType::tag:
      headerSs << "tag" << '\0';
      break;

    case HashType::tree:
      headerSs << "tree" << '\0';
      break;
  }

  std::string header = headerSs.str( );
  size_t headerSize = header.size( );

  hashObj.addData( reinterpret_cast< uint8_t * >( &header[ 0 ] ), 0, static_cast< unsigned int >( headerSize ) );

  // Read input into buffer and write buffer to hash function.
  do
  {
    memset( &buffer[ 0 ], 0, sizeof( buffer ) );
    inputStream_.read( reinterpret_cast< char * >( &buffer[ 0 ] ), sizeof( buffer ) );
    if ( std::streamsize bytesRead = inputStream_.gcount( );
          bytesRead > 0 )
    {
      hashObj.addData( &buffer[ 0 ], 0, static_cast< unsigned int>( bytesRead ) );
    }
  } while ( inputStream_.good( ) );

  inputStream_.clear( );

  std::vector< unsigned char > op = hashObj.digest();

  for ( auto &oi : op )
  {
    Hex( oi, [ &hashSs ] ( unsigned char a_ )
    {
      hashSs << a_;
    } );
  }

  {
    inputStream_.seekg( std::ios_base::beg );

    std::filesystem::path objectPath = dvs_.GetDvsDirectory( );
    objectPath /= "objects";
    objectPath /= hashSs.str( ).substr( 0, 2 );
    std::string shardedHash = hashSs.str( ).substr( 2 );

    std::filesystem::create_directories( objectPath );

    objectPath /= shardedHash;

    if ( std::filesystem::exists( objectPath ) )
    {
      std::stringstream ss;
      ss << "Warning: File '" << objectPath << "' already exists.";
      std::cerr << ss.str( ) << std::endl;
      return { "", hashSs.str( ) };
    }

    std::ofstream outputFile( objectPath,  std::ios_base::binary );

    outputFile.write( &header[ 0 ], headerSize );

    while ( outputFile.good( ) )
    {
      memset( &buffer[ 0 ], 0, sizeof( buffer ) );
      inputStream_.read( reinterpret_cast< char * >( &buffer[ 0 ] ), sizeof( buffer ) );
      if ( std::streamsize bytesRead = inputStream_.gcount( );
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


std::string HashCommand::LookupType( const HashType hashType_ )
{
  return s_HashMapLookup[ hashType_ ];
}
