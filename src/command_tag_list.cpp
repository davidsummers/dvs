#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>

#include "command_tag_list.h"
#include "dvs.h"

Error ListTagCommand::ParseArgs( DocOptArgs & /* args_ */ )
{
  return "";
}

Error ListTagCommand::operator( )( DVS &dvs_ )
{
  if ( Error validateError = dvs_.Validate( ); !validateError.empty( ) )
  {
    return validateError;
  }

  Error err = ListTag( dvs_ );

  return err;
}

Error ListTagCommand::ListTag( DVS &dvs_ )
{
  Error err;

  std::filesystem::path tagPath = dvs_.GetSpecialPath( SpecialName::TAGS );

  if ( !std::filesystem::exists( tagPath ) )
  {
    return ""; // Nothing to do or show here.
  }

  for ( const auto &entry : std::filesystem::directory_iterator( tagPath ) )
  {
    std::cout << entry.path( ).filename( ).string( ) << std::endl;
  }

  return err;
}
