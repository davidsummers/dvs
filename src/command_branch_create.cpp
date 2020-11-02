#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>

#include "command_branch_create.h"
#include "dvs.h"


std::string CreateBranchCommand::ParseArgs( std::map< std::string, docopt::value > &args_ )
{
  std::string err;

  if ( docopt::value hashOption = args_[ "<BranchName>" ];
       hashOption && hashOption.isString( ) && !hashOption.asString( ).empty( ) )
  {
      // m_HashId = hashOption.asString( );
  }
  else
  {
      std::stringstream ss;
      ss << "Missing hash identifier.";
      err = ss.str( );
  }

  return err;
}


std::string CreateBranchCommand::operator ( ) ( DVS &dvs_ )
{
  if ( std::string validateError = dvs_.Validate( );
       !validateError.empty( ) )
  {
    return validateError;
  }

  return "branch create Not Implemented Yet.";
}


std::string CreateBranchCommand::Branch( DVS &dvs_, const std::string &branchName_ )
{
  std::string result;
  std::cout << "Create branch name '" << branchName_ << "'" << std::endl;
  return result;
}
