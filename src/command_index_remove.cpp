#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>

#include "command_index_remove.h"
#include "dvs.h"
#include "index.h"

Error IndexRemoveCommand::ParseArgs( DocOptArgs &args_ )
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
    ss << "Missing path to remove from index.";
    err = ss.str( );
  }

  return err;
}

Error IndexRemoveCommand::operator( )( DVS &dvs_ )
{
  if ( Error validateError = dvs_.Validate( ); !validateError.empty( ) )
  {
    return validateError;
  }

  Error err = IndexRemove( dvs_, m_Pathname );

  return err;
}

Error IndexRemoveCommand::IndexRemove( DVS &dvs_, const std::string &pathName_ )
{
  if ( !std::filesystem::exists( pathName_ ) )
  {
    std::stringstream ss;
    ss << "Specified file '" << pathName_ << "' does not exist.";
    return ss.str( );  
  }

  Index &index = dvs_.GetIndex( );

  Error err = index.WithIndex( dvs_, [ &dvs_, &index, &pathName_ ] ( )
  {
    Error err = index.RemoveEntry( dvs_, pathName_ );

    return err;
  } );
  
  return err;
}
