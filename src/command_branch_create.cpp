#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>

#include "command_branch_create.h"
#include "dvs.h"

Error CreateBranchCommand::ParseArgs( std::map< std::string, docopt::value > &args_ )
{
  Error err;

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

Error CreateBranchCommand::operator( )( DVS &dvs_ )
{
  if ( Error err = dvs_.Validate( ); !err.empty( ) )
  {
    return err;
  }

  return CreateBranch( dvs_, m_BranchName, m_Oid );
}

Error CreateBranchCommand::CreateBranch( DVS &dvs_, const std::string &branchName_, const Oid &oid_ )
{
  Error err;
  std::string oid = dvs_.GetOid( oid_ );
  dvs_.SetRef( s_REFS_BRANCHES_LOCAL + branchName_, RefValue{ false, oid } );
  std::cout << "Create branch name '" << branchName_ << "'" << std::endl;
  return err;
}
