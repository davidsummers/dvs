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

#if 0
  OidResult result = ReadTree( dvs_, m_HashId );
#endif

  return "Not Yet Implemented.";
}

#if 0
OidResult ReadTreeCommand::ReadTree( DVS &dvs_, const std::string &hashId_ )
{
  ReadTreeGenerator readTreeGenerator;

  for ( auto [ err, path, hash ] : readTreeGenerator.GetTree( hashId_, "./" ) )
  {
    std::filesystem::create_directories( path.dirname( ) );
  }

  return { "Not Yet Implemented.", "" };
}
#endif