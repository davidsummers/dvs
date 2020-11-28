#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>

#include "command_tag_delete.h"
#include "dvs.h"

Error DeleteTagCommand::ParseArgs( DocOptArgs &args_ )
{
  Error err;

  if ( docopt::value tagOption = args_[ "<tag>" ];
       tagOption && tagOption.isString( ) && !tagOption.asString( ).empty( ) )
  {
    m_TagName = tagOption.asString( );
  }
  else
  {
    std::stringstream ss;
    ss << "Missing tag identifier.";
    err = ss.str( );
  }

  return err;
}

Error DeleteTagCommand::operator( )( DVS &dvs_ )
{
  if ( Error validateError = dvs_.Validate( ); !validateError.empty( ) )
  {
    return validateError;
  }

  Error err = DeleteTag( dvs_, m_TagName );

  return err;
}

Error DeleteTagCommand::DeleteTag( DVS &dvs_, const std::string &tagName_ )
{
  Error err;

  std::filesystem::path tagPath = dvs_.GetDvsDirectory( ) / s_REFS_TAGS / tagName_;

  if ( !std::filesystem::exists( tagPath ) )
  {
    std::stringstream ss;
    ss << "Tag '" << tagName_ << "' does not exist.";
    err = ss.str( );
    return err;
  }

  std::filesystem::remove( tagPath );

  return err;
}
