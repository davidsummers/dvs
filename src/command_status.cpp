#include <cassert>
#include <iostream>
#include <sstream>

#include "command_status.h"
#include "command_write_tree.h"
#include "diff.h"
#include "dvs.h"
#include "index.h"
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
  OidResult result;

  // First show what branch we are on.
  if ( result = ShowBranch( dvs_ );
       !result.has_value( ) )
  {
    return result.error( );
  }

  // Get IndexTree Oid.
  OidResult indexTreeResult;
  WriteTreeCommand writeTreeCommand;

  // Write tree from index and return Oid.
  if ( indexTreeResult = writeTreeCommand.WriteTreeFromIndex( dvs_ );
       !indexTreeResult.has_value( ) )
  {
    return indexTreeResult.error( );
  }

  // Next show changed files from HEAD to Index.
  if ( result = DiffHeadToIndex( dvs_, indexTreeResult.value( ) );
       !result.has_value( ) )
  {
    return result.error( );
  }

  // Next show changed files from Index to Current Directory.
  if ( result = DiffIndexToCurrentDirectory( dvs_, indexTreeResult.value( ) );
       !result.has_value( ) )
  {
    return result.error( );
  }

  return result.value( );
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


Error StatusCommand::ShowBranch( DVS &dvs_ )
{
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

  std::cout << std::endl;

  return "";
}

OidResult StatusCommand::DiffHeadToIndex( DVS &dvs_, const Oid &indexOid_ )
{
  OidResult result;

  Oid headRef = dvs_.GetOid( dvs_.GetSpecialName( SpecialName::HEAD ) );
  headRef = dvs_.GetOid( headRef );

  CommitRecord commitRecord;

  if ( result = commitRecord.Read( dvs_, headRef );
       !result.has_value( ) )
  {
    return result;
  }

  TreeRecord headTree;

  if ( result = headTree.Read( dvs_, commitRecord.GetTreeOid( ) );
       !result.has_value( ) )
  {
    return result;
  }

  TreeRecord indexTree;

  if ( result = indexTree.Read( dvs_, indexOid_ );
       !result.has_value( ) )
  {
    return result.error( );
  }
  
  std::cout << "Changes to be commited from index:" << std::endl;

  result = Diff::ListChangedFiles( dvs_, headTree, indexTree );

  std::cout << std::endl;

  return result;
}

OidResult StatusCommand::DiffIndexToCurrentDirectory( DVS &dvs_, const Oid &indexOid_ )
{
  OidResult result;

  std::string path = dvs_.GetTopLevelDirectory( ).string( );

  WriteTreeCommand writeTreeCommand;
  OidResult currentDirectoryTreeResult = writeTreeCommand.WriteTreeFromDirectory( dvs_, path );

  if ( !currentDirectoryTreeResult.has_value( ) )
  {
    return currentDirectoryTreeResult;
  }

  Error err;

  TreeRecord currentDirectoryTree;

  if ( result = currentDirectoryTree.Read( dvs_, currentDirectoryTreeResult.value( ) );
       !result.has_value( ) )
  {
    return result;
  }

  TreeRecord indexTree;

  if ( result = indexTree.Read( dvs_, indexOid_ );
       !result.has_value( ) )
  {
    return result;
  }

  std::cout << "Changes not staged for commit from index:" << std::endl;

  result = Diff::ListChangedFiles( dvs_, indexTree, currentDirectoryTree );

  std::cout << std::endl;

  return result;  
}
