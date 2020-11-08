#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>

#include "command_tag.h"
#include "dvs.h"

std::string TagCommand::ParseArgs( std::map< std::string, docopt::value > &args_ )
{
  std::string err;

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

std::string TagCommand::operator( )( DVS &dvs_ )
{
  if ( std::string validateError = dvs_.Validate( ); !validateError.empty( ) )
  {
    return validateError;
  }

  std::string result = Tag( dvs_, m_TagName, m_HashId );

  return result;
}

std::string TagCommand::Tag( DVS &dvs_, const std::string &tagName_, const std::string &hashId_ )
{
  std::string result;

  RefValue refValue = RefValue{ false, hashId_ };

  if ( refValue.value.empty( ) )
  {
    refValue = dvs_.GetRef( s_HEAD_REF );
  }

  dvs_.SetRef( s_REFS_TAGS + tagName_, refValue );

  return result;
}
