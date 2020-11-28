#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>

#include "command_diff.h"
#include "command_hash.h"
#include "command_write_tree.h"
#include "diff.h"
#include "dvs.h"
#include "record_commit.h"

Error DiffCommand::ParseArgs( DocOptArgs & /* args_ */ )
{
  Error err;

  return err;
}

Error DiffCommand::operator( )( DVS &dvs_ )
{
  if ( Error validateError = dvs_.Validate( ); !validateError.empty( ) )
  {
    return validateError;
  }

  Error err = Diff( dvs_ );

  return err;
}

Error DiffCommand::Diff( DVS &dvs_ )
{
  Error err;

  WriteTreeCommand currentTreeCommand;
  OidResult currentTreeResult = currentTreeCommand.WriteTree( dvs_, "." );

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

  RefValue parentRef = dvs_.GetRef( s_HEAD_REF );

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

  err = Diff::DiffTrees( dvs_, parentTree, currentTree );  

  return err;
}
