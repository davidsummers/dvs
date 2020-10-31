#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>

#include "command_log.h"
#include "command_hash.h"
#include "dvs.h"


std::string LogCommand::ParseArgs( std::map< std::string, docopt::value > &args_ )
{
  std::string err;

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


std::string LogCommand::operator ( ) ( DVS &dvs_ )
{
  if ( std::string validateError = dvs_.Validate( );
       !validateError.empty( ) )
  {
    return validateError;
  }

  std::string result = GetLog( dvs_, m_HashId );

  return result;
}


std::string LogCommand::GetLog( DVS &dvs_, const std::string &hashId_ )
{
  std::string result;

  std::string hashId = hashId_;

  if ( hashId.empty( ) )
  {
    hashId = dvs_.GetHead( );
  }

  return result;
}
