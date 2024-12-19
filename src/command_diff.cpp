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

  if ( result = commitRecord.Read( dvs_, headRef );
       !result.has_value( ) )
  {
    return result.error( );
  }

  TreeRecord headTree;

  if ( result = headTree.Read( dvs_, commitRecord.GetTreeOid( ) );
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

  TreeRecord indexTree;

  if ( result = indexTree.Read( dvs_, indexTreeResult.value( ) );
       !result.has_value( ) )
  {
    return result.error( );
  }
  
  result = Diff::DiffTrees( dvs_, headTree, indexTree );

  return result.has_value( ) ? "" : result.error( );
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

  if ( !currentTreeResult.has_value( ) )
  {
    return currentTreeResult.error( );
  }

  TreeRecord currentTree;
  err = currentTree.Read( dvs_, currentTreeResult.value( ) );

  if ( !err.empty( ) )
  {
    return err;
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

  TreeRecord indexTree;

  if ( err = indexTree.Read( dvs_, indexTreeResult.value( ) );
       !err.empty( ) )
  {
    return err;
  }

  err = Diff::DiffTrees( dvs_, indexTree, currentTree );  

  return err;
}
