#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>

#include "command_cat.h"
#include "dvs.h"

Error CatCommand::ParseArgs( std::map< std::string, docopt::value > &args_ )
{
  Error err;

  if ( docopt::value sizeOption = args_[ "-s" ]; sizeOption && sizeOption.isBool( ) && sizeOption.asBool( ) )
  {
    m_PrintType = PrintType::size;
  }

  if ( docopt::value typeOption = args_[ "-t" ]; typeOption && typeOption.isBool( ) && typeOption.asBool( ) )
  {
    m_PrintType = PrintType::type;
  }

  if ( docopt::value hashOption = args_[ "<hash>" ];
       hashOption && hashOption.isString( ) && !hashOption.asString( ).empty( ) )
  {
    m_HashId = hashOption.asString( );
  }
  else
  {
    std::stringstream ss;
    ss << "Missing hash identifier.";
    err = ss.str( );
  }

  return err;
}

Error CatCommand::operator( )( DVS &dvs_ )
{
  if ( Error validateError = dvs_.Validate( ); !validateError.empty( ) )
  {
    return validateError;
  }

  std::ostream *outputStream = &std::cout;

  if ( m_PrintType == PrintType::size || m_PrintType == PrintType::type )
  {
    outputStream = nullptr;
  }

  CatResult result = GetHash( dvs_, m_HashId, outputStream );

  if ( m_PrintType == PrintType::type )
  {
    std::cout << result.type << std::endl;
  }

  if ( m_PrintType == PrintType::size )
  {
    std::cout << result.size << std::endl;
  }

  return result.err;
}

CatCommand::CatResult CatCommand::GetHash( DVS &              dvs_,
                                           const std::string &hashId_,
                                           std::ostream *     ostream_,
                                           RecordType         expectedRecordType_ )
{
  CatResult result;

  std::string hashId = hashId_;

  hashId = dvs_.GetOid( hashId );

  std::filesystem::path hashPath;

  if ( !hashId.empty( ) )
  {
    hashPath = dvs_.GetDvsDirectory( ) / "objects" / hashId.substr( 0, 2 ) / hashId.substr( 2 );
  }

  if ( !std::filesystem::exists( hashPath ) )
  {
    std::stringstream ss;
    ss << "Hash " << hashId << " does not exist.";
    result.err = ss.str( );
    return result;
  }

  std::ifstream inputFile( hashPath, std::ios_base::binary );

  if ( !inputFile.is_open( ) )
  {
    std::stringstream ss;
    ss << "Can't open file " << hashPath << ".";
    result.err = ss.str( );
    return result;
  }

  std::string header;

  // Read header.
  std::getline( inputFile, header, '\0' );

  {
    std::string::size_type pos = header.find( ' ' );
    std::string            sizeStr;

    if ( pos != std::string::npos )
    {
      sizeStr = header.substr( pos + 1 );
      header  = header.substr( 0, pos );
    }

    result.size = atoi( sizeStr.c_str( ) );
    result.type = header;
  }

  if ( expectedRecordType_ != RecordType::none )
  {
    if ( expectedRecordType_ == RecordType::blob && result.type != "blob" )
    {
      std::stringstream ss;
      ss << "Expected type 'blob' but got '" << result.type << "'" << std::endl;
      result.err = ss.str( );
      return result;
    }

    if ( expectedRecordType_ == RecordType::commit && result.type != "commit" )
    {
      std::stringstream ss;
      ss << "Expected type 'commit' but got '" << result.type << "'" << std::endl;
      result.err = ss.str( );
      return result;
    }

    if ( expectedRecordType_ == RecordType::tag && result.type != "tag" )
    {
      std::stringstream ss;
      ss << "Expected type 'tag' but got '" << result.type << "'" << std::endl;
      result.err = ss.str( );
      return result;
    }

    if ( expectedRecordType_ == RecordType::tree && result.type != "tree" )
    {
      std::stringstream ss;
      ss << "Expected type 'tree' but got '" << result.type << "'" << std::endl;
      result.err = ss.str( );
      return result;
    }
  }

  if ( ostream_ == nullptr )
  {
    return result;
  }

  uint8_t buffer[ 4096 ];

  do
  {
    inputFile.read( reinterpret_cast< char * >( &buffer[ 0 ] ), sizeof( buffer ) );
    if ( std::streamsize bytesRead = inputFile.gcount( ); bytesRead > 0 )
    {
      ostream_->write( reinterpret_cast< const char * >( &buffer[ 0 ] ), bytesRead );
    }
  } while ( inputFile.good( ) );

  return result;
}

CatCommand::CatResult CatCommand::GetStreamFromOid( DVS &dvs_, const std::optional< Oid > &oid_, std::string &filename_, std::ifstream &inputStream_ )
{
  CatCommand::CatResult result;

  if ( !oid_.has_value( ) )
  {
    result.err = "Oid has no value.";
    return result;
  }

  std::filesystem::path hashPath = dvs_.GetDvsDirectory( ) / "objects" / oid_.value( ).substr( 0, 2 ) / oid_.value( ).substr( 2 );

  filename_ = hashPath.string( );

  if ( !std::filesystem::exists( filename_ ) )
  {
    std::stringstream ss;
    ss << "Hash " << oid_.value( ) << " does not exist.";
    result.err = ss.str( );
    return result;
  }

  inputStream_.open( filename_, std::ios_base::binary );

  if ( !inputStream_.is_open( ) )
  {
    std::stringstream ss;
    ss << "Can't open file " << filename_ << ".";
    result.err = ss.str( );
    return result;
  }

  std::string header;

  // Read header.
  std::getline( inputStream_, header, '\0' );

  {
    std::string::size_type pos = header.find( ' ' );
    std::string            sizeStr;

    if ( pos != std::string::npos )
    {
      sizeStr = header.substr( pos + 1 );
      header  = header.substr( 0, pos );
    }

    result.size = atoi( sizeStr.c_str( ) );
    result.type = header;
  }

  return result;
}
