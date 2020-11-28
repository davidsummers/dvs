#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>

#include "command_tag_create.h"
#include "dvs.h"

Error CreateTagCommand::ParseArgs( DocOptArgs &args_ )
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

Error CreateTagCommand::operator( )( DVS &dvs_ )
{
  if ( Error validateError = dvs_.Validate( ); !validateError.empty( ) )
  {
    return validateError;
  }

  Error err = CreateTag( dvs_, m_TagName, m_HashId );

  return err;
}

Error CreateTagCommand::CreateTag( DVS &dvs_, const std::string &tagName_, const std::string &hashId_ )
{
  Error err;

  RefValue refValue = RefValue{ false, hashId_ };

  if ( refValue.value.empty( ) )
  {
    refValue = dvs_.GetRef( dvs_.GetSpecialName( SpecialName::HEAD ) );
  }

  if ( refValue.value.empty( ) )
  {
    err = "Couldn't find HEAD reference. Is this an empty repository?";
    return err;
  }

  dvs_.SetRef( dvs_.GetSpecialName( SpecialName::TAGS ) + tagName_, refValue );

  return err;
}
