#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>

#include "command_cat.h"
#include "dvs.h"


std::string CatCommand::operator ( ) ( DVS &dvs_, const PrintType printType_, const std::string &hash_id_ )
{
  if ( std::string validateError = dvs_.Validate( );
       !validateError.empty( ) )
  {
    return validateError;
  }

  std::filesystem::path hashPath = dvs_.GetDvsDirectory( ) / "objects" / hash_id_.substr( 0, 2 ) / hash_id_.substr( 2 );

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

  std::string header;

  // Read header.
  std::getline( inputFile, header, '\0' );

  if ( printType_ == PrintType::type )
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
