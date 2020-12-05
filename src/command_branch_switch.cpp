#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>

#include "command_branch_switch.h"
#include "command_cat.h"
#include "command_read_tree.h"
#include "dvs.h"
#include "index.h"
#include "record_commit.h"

Error SwitchBranchCommand::ParseArgs( DocOptArgs &args_ )
{
  Error err;

  if ( docopt::value branchNameOption = args_[ "<BranchName>" ];
       branchNameOption && branchNameOption.isString( ) && !branchNameOption.asString( ).empty( ) )
  {
    m_BranchName = branchNameOption.asString( );
  }
  else
  {
    err = "Missing required branch name.";
  }

  return err;
}

Error SwitchBranchCommand::operator( )( DVS &dvs_ )
{
  if ( Error validateError = dvs_.Validate( ); !validateError.empty( ) )
  {
    return validateError;
  }

  Error err = Switch( dvs_, m_BranchName );

  return err;
}

Error SwitchBranchCommand::Switch( DVS &dvs_, const std::string &branchName_ )
{
  Error err;

  Oid oid = dvs_.GetOid( branchName_ );

  RefValue refValue{ false, oid };

  if ( refValue.value.empty( ) )
  {
    refValue = dvs_.GetRef( dvs_.GetSpecialName( SpecialName::HEAD ) );
  }

  if ( refValue.value.empty( ) )
  {
    return "Expected non-empty Hash ID.";
  }

  refValue.value = dvs_.GetOid( refValue.value );

  CommitRecord commit;

  if ( err = commit.Read( dvs_, refValue.value ); !err.empty( ) )
  {
    return err;
  }

  // Remove the existing index file since we are about to replace it.
  std::filesystem::remove( dvs_.GetSpecialPath( SpecialName::INDEX ) );

  Index &index = dvs_.GetIndex( );

  ReadTreeCommand readTreeCommand;
  OidResult       readTreeResult;

  // clang-format off
  err = index.WithIndex( dvs_, [ &dvs_, &commit, &index, &readTreeCommand, &readTreeResult ] ( ) -> Error
  {
    // Now read the resulting tree.
    readTreeResult = readTreeCommand.ReadTreeToIndex( dvs_, index, commit.GetTreeOid( ) );

    if ( !readTreeResult.err.empty( ) )
    {
      return readTreeResult.err;
    }

    return "";
  } );
  // clang-format on

  readTreeResult = readTreeCommand.ReadTreeToDirectory( dvs_, commit.GetTreeOid( ) );

  if ( !readTreeResult.err.empty( ) )
  {
    return readTreeResult.err;
  }

  if ( IsBranch( dvs_, branchName_ ) )
  {
    std::string branchName = dvs_.GetSpecialName( SpecialName::BRANCHES_LOCAL );
    branchName += branchName_;
    refValue = RefValue{ true, branchName };
  }
  else
  {
    refValue = RefValue{ false, oid };
  }

  // Now save the commit hash in .dvs/HEAD
  const bool deref = false;
  dvs_.SetRef( dvs_.GetSpecialName( SpecialName::HEAD ), refValue, deref );

  return err;
}

bool SwitchBranchCommand::IsBranch( DVS &dvs_, const std::string &branchName_ )
{
  RefValue refValue = dvs_.GetRef( dvs_.GetSpecialName( SpecialName::BRANCHES_LOCAL ) + branchName_ );
  bool     isBranch = !refValue.value.empty( );
  return isBranch;
}
