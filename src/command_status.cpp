#include <iostream>
#include <sstream>

#include "command_status.h"
#include "dvs.h"


std::string StatusCommand::operator ( ) ( DVS &dvs_ )
{
  if ( std::string validate_error = dvs_.Validate( );
       !validate_error.empty( ) )
  {
    std::stringstream ss;
    ss << "Can't validate " << s_DVS_DIR << " directory: " + validate_error;
    return ss.str( );
  }

  std::cout << "Status: " << std::endl;
  return ""; // No error.
}
