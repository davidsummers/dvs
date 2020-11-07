#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>

#include "command_branch_delete.h"
#include "dvs.h"


std::string DeleteBranchCommand::ParseArgs( std::map< std::string, docopt::value > &args_ )
{
  std::string err;

  if ( docopt::value branchName = args_[ "<BranchName>" ];
       branchName && branchName.isString( ) && !branchName.asString( ).empty( ) )
  {
    m_Branch = branchName.asString( );
  }
  else
  {
    err = "Missing branch name.";
  }

  return err;
}


std::string DeleteBranchCommand::operator ( ) ( DVS &dvs_ )
{
  if ( std::string validateError = dvs_.Validate( );
       !validateError.empty( ) )
  {
    return validateError;
  }
  
  return DeleteBranch( dvs_, m_Branch );
}


std::string DeleteBranchCommand::DeleteBranch( DVS &dvs_, const std::string &branchName_ )
{
  std::string result;

  std::filesystem::path localBranch = dvs_.GetDvsDirectory( );
  localBranch /= s_REFS_BRANCHES_LOCAL;
  localBranch /= branchName_;

  std::filesystem::path remoteBranch = dvs_.GetDvsDirectory( );
  remoteBranch /= s_REFS_BRANCHES_REMOTE;
  remoteBranch /= branchName_;

  if ( std::filesystem::exists( localBranch ) )
  {
    std::filesystem::remove( localBranch );
    std::cout << "Removing local branch: " << branchName_ << std::endl;
  }
  else if ( std::filesystem::exists( remoteBranch ) )
  {
    std::filesystem::remove( remoteBranch );
    std::cout << "Removing remote branch: " << branchName_ << std::endl;
  }
  else
  {
    std::stringstream ss;
    ss << "Branch '" << branchName_ << "' does not exist." << std::endl;;
    result = ss.str( );
  }

  return result;
}
