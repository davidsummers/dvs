#include <iostream>
#include <fstream>

#include "command_hash.h"
#include "dvs.h"
#include "Hex.h"
#include "Keccak.h"


std::string HashCommand::ParseArgs( std::map< std::string, docopt::value > &args_ )
{
    std::string err;

    if ( docopt::value stdinOption = args_[ "-i" ];
         stdinOption && stdinOption.isBool( ) && stdinOption.asBool( ) )
    {
      m_StdInput = true;
    }

    if ( docopt::value writeOption = args_[ "-w" ];
         writeOption && writeOption.isBool( ) && writeOption.asBool( ) )
    {
      m_WriteFile = true;
    }

    if ( !m_StdInput )
    {
      if ( docopt::value fileOption = args_[ "<file>" ];
           fileOption && fileOption.isString( ) && !fileOption.asString( ).empty( ) )
      {
        m_Filename = fileOption.asString( );
      }
      else
      {
        err = "Missing file name.";
      }
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

  std::string err;

  if ( m_StdInput )
  {
    std::istream *inputStream = &std::cin;

    std::ifstream inputFileStream;

    // Set up output to temporary output file so we can get the file length.
    std::filesystem::path tempPath = std::filesystem::temp_directory_path( );
    std::string t = "XXXXXX";
    tempPath /= mktemp( &t[ 0 ] );
    std::ofstream outputFile( tempPath,  std::ios_base::binary );

    char buffer[ 4096 ];

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

    m_Filename = tempPath.string( );
  }

  {
    std::istream *inputStream = nullptr;

    std::ifstream inputFileStream;

    {
      inputFileStream.open( m_Filename, std::ios_base::binary );

      if ( !inputFileStream.is_open( ) )
      {
        std::stringstream ss;
        ss << "Couldn't open input file '" << m_Filename << "'";
        err = ss.str( );
        return err;
      }

      inputStream = &inputFileStream;
    }

    const int SHA_BUF_SIZE = 4096;
    Keccak hashObj( 256 );
    std::vector< uint8_t > buffer( SHA_BUF_SIZE );
  
    // Add header (hash type) to buffer.
    std::stringstream headerSs;

    switch ( m_HashType )
    {
      case HashType::blob:
        headerSs << "blob " << std::filesystem::file_size( m_Filename ) << '\0';
        break;

      case HashType::commit:
        headerSs << "commit\0";
        break;

      case HashType::none:
        return "HashType: none"; // Illegal - return errror.

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

    if ( m_WriteFile )
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
        return ss.str( );
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
  }

  if ( m_StdInput )
  {
    std::filesystem::remove( m_Filename );
  }

  return "";
}
