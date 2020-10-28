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

  ReadTreeResult result = ReadTree( dvs_, m_HashId );

  return result.err;
}

ReadTreeCommand::ReadTreeResult ReadTreeCommand::ReadTree( DVS &dvs_, const std::string &hashId_ )
{
  return { "Not Yet Implemented.", "" };
}
