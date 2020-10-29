#include <filesystem>
#include <iostream>
#include <sstream>

#include "command_hash.h"
#include "command_read_tree.h"
#include "dvs.h"

std::string ReadTreeCommand::ParseArgs( std::map< std::string, docopt::value > &args_ )
{
  std::string err;

  if ( docopt::value hashOption = args_[ "<hash>" ];
        hashOption && hashOption.isString( ) && !hashOption.asString( ).empty( ) )
  {
    m_HashId = hashOption.asString( );
  }
  else
  {
    err = "Missing hash ID.";
  }

  return err;
}


std::string ReadTreeCommand::operator ( ) ( DVS &dvs_ )
{
  if ( std::string validateError = dvs_.Validate( );
       !validateError.empty( ) )
  {
    return validateError;
  }

  OidResult result = ReadTree( dvs_, m_HashId );

  return result.err;
}


OidResult ReadTreeCommand::ReadTree( DVS &dvs_, const std::string &hashId_ )
{
  OidResult result;

#if 0
  ReadTreeGenerator readTreeGenerator;

  for ( auto [ err, path, hash ] : readTreeGenerator.GetTree( hashId_, "./" ) )
  {
    std::filesystem::create_directories( path.dirname( ) );
  }
#endif
  result.err = "Not Yet Implemented.";
  return result;
}
