#include <fstream>
#include <iostream>
#ifdef WIN32
#include <io.h>
#define mktemp _mktemp
#endif

#include "Hex.h"
#include "Keccak.h"
#include "command_hash.h"
#include "dvs.h"

using RecordMapLookup = std::map< RecordType, std::string >;

// clang-format off
RecordMapLookup s_RecordMapLookup =
{
  { RecordType::none, "none" },
  { RecordType::blob, "blob" },
  { RecordType::tag,  "tag"  },
  { RecordType::tree, "tree" },
};
//clang-format on


Error HashCommand::ParseArgs( DocOptArgs &args_ )
{
  Error err;

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

Error HashCommand::operator( )( DVS &dvs_ )
{
  if ( Error validate_error = dvs_.Validate( ); !validate_error.empty( ) )
  {
    return validate_error;
  }

  auto [ err, oid ] = Hash( dvs_, m_Filename );

  std::cout << oid << std::endl;

  return err;
}

OidResult HashCommand::Hash( DVS &dvs_, const std::string &filename_, const RecordType hashType_ )
{
  OidResult result;
  std::ifstream inputFileStream( filename_, std::ios_base::binary );

  if ( !inputFileStream.is_open( ) )
  {
    std::stringstream ss;
    ss << "Couldn't open input file '" << filename_ << "'";
    result.err = ss.str( );

    return result;
  }

  size_t size = std::filesystem::file_size( filename_ );

  result = Hash( dvs_, inputFileStream, size, hashType_ );

  return result;
}

OidResult HashCommand::Hash( DVS &dvs_, std::istream &inputStream_, size_t size_, RecordType recordType_ )
{
  OidResult result;
  std::ostringstream hashSs;

  const int SHA_BUF_SIZE = 4096;
  Keccak hashObj( 256 );
  std::vector< uint8_t > buffer( SHA_BUF_SIZE );

  // Add header (hash type) to buffer.
  std::stringstream headerSs;

  switch ( recordType_ )
  {
    case RecordType::blob:
      headerSs << "blob " << size_ << '\0';
      break;

    case RecordType::commit:
      headerSs << "commit" << '\0';
      break;

    case RecordType::none:
      result.err = "HashType: none"; // Illegal - return errror.
      return result;

    case RecordType::tag:
      headerSs << "tag" << '\0';
      break;

    case RecordType::tree:
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
    if ( std::streamsize bytesRead = inputStream_.gcount( ); bytesRead > 0 )
    {
      hashObj.addData( &buffer[ 0 ], 0, static_cast< unsigned int >( bytesRead ) );
    }
  } while ( inputStream_.good( ) );

  inputStream_.clear( );

  std::vector< unsigned char > op = hashObj.digest( );

  for ( auto &oi : op )
  {
    Hex( oi, [ &hashSs ]( unsigned char a_ ) { hashSs << a_; } );
  }

  {
    OidResult result;
    inputStream_.seekg( std::ios_base::beg );

    std::filesystem::path objectPath = dvs_.GetDvsDirectory( );
    objectPath /= "objects";
    objectPath /= hashSs.str( ).substr( 0, 2 );
    std::string shardedHash = hashSs.str( ).substr( 2 );

    std::filesystem::create_directories( objectPath );

    objectPath /= shardedHash;

    if ( std::filesystem::exists( objectPath ) )
    {
#if 0
      std::stringstream ss;
      ss << "Warning: File '" << objectPath << "' already exists.";
      std::cerr << ss.str( ) << std::endl;
#endif
      result.oid = hashSs.str( );
      return result;
    }

    std::ofstream outputFile( objectPath, std::ios_base::binary );

    outputFile.write( &header[ 0 ], headerSize );

    while ( outputFile.good( ) )
    {
      memset( &buffer[ 0 ], 0, sizeof( buffer ) );
      inputStream_.read( reinterpret_cast< char * >( &buffer[ 0 ] ), sizeof( buffer ) );
      if ( std::streamsize bytesRead = inputStream_.gcount( ); bytesRead > 0 )
      {
        outputFile.write( reinterpret_cast< const char * >( &buffer[ 0 ] ), bytesRead );
      }
      else
      {
        break;
      }
    }
  }

  result.oid = hashSs.str( );
  return result;
}

std::string HashCommand::LookupType( const RecordType recordType_ )
{
  return s_RecordMapLookup[ recordType_ ];
}


RecordType HashCommand::LookupType( const std::string &type_ )
{
  for ( const auto &entry : s_RecordMapLookup )
  {
    if ( entry.second == type_ )
    {
      return entry.first;
    }
  }

  return RecordType::none;
}
