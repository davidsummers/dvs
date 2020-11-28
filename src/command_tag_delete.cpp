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

  if ( docopt::value hashOption = args_[ "<hash>" ];
       hashOption && hashOption.isString( ) && !hashOption.asString( ).empty( ) )
  {
    m_HashId = hashOption.asString( );
  }

  return err;
}

Error DeleteTagCommand::operator( )( DVS &dvs_ )
{
  if ( Error validateError = dvs_.Validate( ); !validateError.empty( ) )
  {
    return validateError;
  }

  Error err = Tag( dvs_, m_TagName, m_HashId );

  return err;
}

Error DeleteTagCommand::Tag( DVS &dvs_, const std::string &tagName_, const std::string &hashId_ )
{
  Error err;

  RefValue refValue = RefValue{ false, hashId_ };

  if ( refValue.value.empty( ) )
  {
    refValue = dvs_.GetRef( s_HEAD_REF );
  }

  dvs_.SetRef( s_REFS_TAGS + tagName_, refValue );

  return err;
}
