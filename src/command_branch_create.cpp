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
      m_BranchName = hashOption.asString( );
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
  
  return CreateBranch( dvs_, m_BranchName, m_Oid );
}


std::string CreateBranchCommand::CreateBranch( DVS &dvs_, const std::string &branchName_, const std::string &oid_ )
{
  std::string result;
  std::string oid = dvs_.GetOid( oid_ );
  dvs_.SetRef( s_REFS_BRANCHES_LOCAL + branchName_, RefValue{ false, oid } );
  std::cout << "Create branch name '" << branchName_ << "'" << std::endl;
  return result;
}
