#include <cassert>
#include <iostream>
#include <sstream>

#include "command_status.h"
#include "command_write_tree.h"
#include "diff.h"
#include "dvs.h"
#include "record_commit.h"
#include "record_tree.h"

Error StatusCommand::ParseArgs( DocOptArgs &args_ )
{
  return "";
}

Error StatusCommand::operator( )( DVS &dvs_ )
{
  if ( Error validate_error = dvs_.Validate( ); !validate_error.empty( ) )
  {
    std::stringstream ss;
    ss << "Can't validate " << dvs_.GetSpecialName( SpecialName::DVS ) << " directory: " + validate_error;
    return ss.str( );
  }

  Error err = Status( dvs_ );

  return err;
}

Error StatusCommand::Status( DVS &dvs_, const std::string &path_ )
{
  // First show which branch we are on.
  std::cout << "Status: " << std::endl;

  std::string head = dvs_.GetOid( "@" );

  if ( head.empty( ) )
  {
    return "Can't find head branch.";
  }

  std::string branch = GetBranchName( dvs_ );

  if ( !branch.empty( ) )
  {
    std::cout << "On branch " << branch << std::endl;
  }
  else
  {
    std::cout << "HEAD detached at " << head << std::endl;
  }

  // Next show changed files.

  std::string path = path_.empty( ) ? dvs_.GetTopLevelDirectory( ).string( ) : path_;

  WriteTreeCommand currentTreeCommand;
  OidResult currentTreeResult = currentTreeCommand.WriteTreeFromDirectory( dvs_, path );

  Error err;

  if ( !currentTreeResult.err.empty( ) )
  {
    err = currentTreeResult.err;
    return err;
  }

  TreeRecord currentTree;
  err = currentTree.Read( dvs_, currentTreeResult.oid );

  if ( !err.empty( ) )
  {
    return err;
  } 

  RefValue parentRef = dvs_.GetRef( dvs_.GetSpecialName( SpecialName::HEAD ) );

  CommitRecord parentCommit;
  err = parentCommit.Read( dvs_, parentRef.value );

  if ( !parentRef.value.empty( ) && !err.empty( ) )
  {
    return err;
  }

  TreeRecord parentTree;

  err = parentTree.Read( dvs_, parentCommit.GetTreeOid( ) );

  if ( !parentCommit.GetTreeOid( ).empty( ) && !err.empty( ) )
  {
    return err;
  }

  err = Diff::ListChangedFiles( dvs_, parentTree, currentTree );  

  return err;
}

std::string StatusCommand::GetBranchName( DVS &dvs_ )
{
  const bool deref = false;
  RefValue   head  = dvs_.GetRef( dvs_.GetSpecialName( SpecialName::HEAD ), deref );

  if ( !head.symbolic )
  {
    return "";
  }

  assert( ( (void) "Head starts with refs/locals", head.value.find( dvs_.GetSpecialName( SpecialName::BRANCHES_LOCAL ) ) == 0 ) );

  std::string result = head.value.substr( head.value.find( dvs_.GetSpecialName( SpecialName::BRANCHES_LOCAL ) ) );

  return result;
}
