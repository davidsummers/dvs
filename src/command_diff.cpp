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

Error DiffCommand::ParseArgs( DocOptArgs &args_ )
{
  Error err;

  if ( docopt::value indexOption = args_[ "--index" ];
       indexOption && indexOption.isBool( ) && indexOption.asBool( ) )
  {
    m_IndexOption = true;
  }

  if ( docopt::value path = args_[ "<path>" ];
       path && path.isString( ) && !path.asString( ).empty( ) )
  {
    m_Path = path.asString( );
  }

  return err;
}

Error DiffCommand::operator( )( DVS &dvs_ )
{
  if ( Error validateError = dvs_.Validate( ); !validateError.empty( ) )
  {
    return validateError;
  }

  Error err = Diff( dvs_, m_Path, m_IndexOption );

  return err;
}

Error DiffCommand::Diff( DVS &dvs_, const std::string &path_, const bool &indexOption_ )
{
  if ( indexOption_ )
  {
    return DiffIndex( dvs_, path_ );
  }
  else
  {
    return DiffWorkingCopy( dvs_, path_ );
  }
}

Error DiffCommand::DiffIndex( DVS &dvs_, const std::string &path_ )
{
  OidResult result;

  Oid headRef = dvs_.GetOid( dvs_.GetSpecialName( SpecialName::HEAD ) );
  headRef = dvs_.GetOid( headRef );

  CommitRecord commitRecord;

  if ( result.err = commitRecord.Read( dvs_, headRef );
       !result.err.empty( ) )
  {
    return result.err;
  }

  TreeRecord headTree;

  if ( result.err = headTree.Read( dvs_, commitRecord.GetTreeOid( ) );
       !result.err.empty( ) )
  {
    return result.err;
  }

  // Get IndexTree Oid.
  OidResult indexTreeResult;
  WriteTreeCommand writeTreeCommand;

  // Write tree from index and return Oid.
  if ( indexTreeResult = writeTreeCommand.WriteTreeFromIndex( dvs_ );
       !indexTreeResult.err.empty( ) )
  {
    return indexTreeResult.err;
  }

  TreeRecord indexTree;

  if ( result.err = indexTree.Read( dvs_, indexTreeResult.oid );
       !result.err.empty( ) )
  {
    return result.err;
  }
  
  result.err = Diff::DiffTrees( dvs_, headTree, indexTree );

  return result.err;
}

Error DiffCommand::DiffWorkingCopy( DVS &dvs_, const std::string &path_ )
{
  Error err;

  std::string path = path_.empty( ) ? dvs_.GetTopLevelDirectory( ).string( ) : path_;

  if ( !std::filesystem::is_directory( path ) )
  {
    std::stringstream ss;
    ss << "DIFF for file: Not Yet Implemented." << std::endl;
    return ss.str( );
  }

  WriteTreeCommand currentTreeCommand;
  OidResult currentTreeResult = currentTreeCommand.WriteTreeFromDirectory( dvs_, path );

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

  // Get IndexTree Oid.
  OidResult indexTreeResult;
  WriteTreeCommand writeTreeCommand;

  // Write tree from index and return Oid.
  if ( indexTreeResult = writeTreeCommand.WriteTreeFromIndex( dvs_ );
       !indexTreeResult.err.empty( ) )
  {
    return indexTreeResult.err;
  }

  TreeRecord indexTree;

  if ( err = indexTree.Read( dvs_, indexTreeResult.oid );
       !err.empty( ) )
  {
    return err;
  }

  err = Diff::DiffTrees( dvs_, indexTree, currentTree );  

  return err;
}
