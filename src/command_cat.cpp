#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>

#include "command_cat.h"
#include "dvs.h"


std::string CatCommand::ParseArgs( std::map< std::string, docopt::value > &args_ )
{
  std::string err;

  if ( docopt::value typeOption = args_[ "-t" ];
        typeOption && typeOption.isBool( ) && typeOption.asBool( ) )
  {
    m_PrintType = CatCommand::PrintType::type; 
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


std::string CatCommand::operator ( ) ( DVS &dvs_ )
{
  if ( std::string validateError = dvs_.Validate( );
       !validateError.empty( ) )
  {
    return validateError;
  }

  std::string err = GetHash( dvs_, m_HashId );

  return err;
}


std::string CatCommand::GetHash( DVS &dvs_, const std::string &hashId_ )
{
  std::filesystem::path hashPath = dvs_.GetDvsDirectory( ) / "objects" / hashId_.substr( 0, 2 ) / hashId_.substr( 2 );

  if ( !std::filesystem::exists( hashPath ) )
  {
    std::stringstream ss;
    ss << "Hash " << hashId_ << " does not exist.";
    return ss.str( );
  }

  std::ifstream inputFile( hashPath, std::ios_base::binary );

  if ( !inputFile.is_open( ) )
  {
    std::stringstream ss;
    ss << "Can't open file " << hashPath << ".";
    return ss.str( );
  }

  std::string header;

  // Read header.
  std::getline( inputFile, header, '\0' );

  if ( m_PrintType == PrintType::type )
  {
    std::string::size_type pos = header.find( ' ' );
    if ( pos != std::string::npos )
    {
      header = header.substr( 0, pos );
    }

    std::cout << header << std::endl;
    return "";
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
