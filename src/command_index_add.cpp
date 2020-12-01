#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>

#include "command_index_add.h"
#include "command_hash.h"
#include "dvs.h"
#include "index.h"

Error IndexAddCommand::ParseArgs( DocOptArgs &args_ )
{
  Error err;

  if ( docopt::value pathOption = args_[ "<path>" ];
       pathOption && pathOption.isString( ) && !pathOption.asString( ).empty( ) )
  {
    m_Pathname = pathOption.asString( );
  }
  else
  {
    std::stringstream ss;
    ss << "Missing path to add to index.";
    err = ss.str( );
  }

  return err;
}

Error IndexAddCommand::operator( )( DVS &dvs_ )
{
  if ( Error validateError = dvs_.Validate( ); !validateError.empty( ) )
  {
    return validateError;
  }

  Error err = IndexAdd( dvs_, m_Pathname );

  return err;
}

Error IndexAddCommand::IndexAdd( DVS &dvs_, const std::string &pathName_ )
{
  if ( !std::filesystem::exists( pathName_ ) )
  {
    std::stringstream ss;
    ss << "Specified file '" << pathName_ << "' does not exist.";
    return ss.str( );  
  }

  Index index;

  if ( Error err = index.Read( dvs_ ); !err.empty( ) && err != "Index file does not exist." )
  {
    return err;
  }

  std::string pathName = pathName_;

  for ( size_t pos = pathName.find( '\\' ); pos != std::string::npos; pos = pathName.find( '\\' ) )
  {
    pathName[ pos ] = '/';
  }

  HashCommand hashCommand;

  auto [ err, hash ] = hashCommand.Hash( dvs_, pathName, RecordType::blob );

  if ( !err.empty( ) )
  {
    return err;
  }

  index.AddEntry( pathName, hash );

  if ( err = index.Write( dvs_ ); !err.empty( ) )
  {
    return err;
  }

  return err;
}
