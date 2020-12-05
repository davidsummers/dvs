#include <iostream>
#include <sstream>

#include "command_hash.h"
#include "command_write_tree.h"
#include "dvs.h"
#include "index.h"
#include "record_tree.h"

Error WriteTreeCommand::ParseArgs( DocOptArgs & )
{
  return "";
}

Error WriteTreeCommand::operator( )( DVS &dvs_ )
{
  if ( Error validateError = dvs_.Validate( ); !validateError.empty( ) )
  {
    return validateError;
  }

  OidResult result = WriteTreeFromDirectory( dvs_ );

  std::cout << "Top Level Directory: " << result.oid << std::endl;

  return result.err;
}

OidResult WriteTreeCommand::WriteTreeFromDirectory( DVS &dvs_, const std::string &dir_ )
{
  OidResult result;

  TreeRecord treeRecord;

  for ( auto const &entry : std::filesystem::directory_iterator( dir_ ) )
  {
    if ( dvs_.IsIgnored( entry.path( ) ) )
    {
      continue;
    }

    if ( entry.is_regular_file( ) )
    {
      HashCommand hashCommand;

      auto [ err, hash ] = hashCommand.Hash( dvs_, entry.path( ).string( ), RecordType::blob );

      if ( !err.empty( ) )
      {
        result.err = err;
        return result;
      }

      treeRecord.AddEntry( entry.path( ).filename( ).string( ), RecordType::blob, hash );
    }
    else if ( entry.is_directory( ) )
    {
      OidResult writeResult = WriteTreeFromDirectory( dvs_, entry.path( ).string( ) );
      if ( !writeResult.err.empty( ) )
      {
        result.err = writeResult.err;
        return result;
      }

      treeRecord.AddEntry( entry.path( ).filename( ).string( ), RecordType::tree, writeResult.oid );
    }
    else
    {
      std::stringstream ss;
      ss << "Unknown file type for " << entry.path( ) << ".";
      result.err = ss.str( );
      return result;
    }
  }

  result = treeRecord.Write( dvs_ );

  return result;
}

OidResult WriteTreeCommand::WriteTreeFromIndex( DVS &dvs_, const std::string &dir_ )
{
  OidResult result;

  Index &index = dvs_.GetIndex( );

  // First, read the index.
  if ( Error err = index.Read( dvs_ ); !err.empty( ) )
  {
    result.err = err;
    return result;
  }

  TreeRecord tree;

  // clang-format off
  index.ForAllEntries( [ &tree ] ( const DirEntry &entry_ )
  {
    TreeRecord *currentTree = &tree;

    // Write the file to the database, I think the file is already written so we just have to figure
    // out which directory it is and write that.

    std::filesystem::path path = entry_.filename;

    for ( std::filesystem::path::iterator itr = path.begin( );
          itr != path.end( );
          ++itr )
    {
      std::filesystem::path component = *itr;
      std::filesystem::path::iterator endPath = itr;
      endPath++;
      RecordType type = endPath == path.end( ) ? RecordType::blob : RecordType::tree;
      DirEntry &newEntry = currentTree->AddEntry( component.string( ), type, type == RecordType::tree ? "" : entry_.oid );

      if ( type == RecordType::tree )
      {
        newEntry.m_Tree = new TreeRecord;
        currentTree = newEntry.m_Tree;
      }
    }
  } );
  // clang-format on

  result = ProcessTreeDirectory( dvs_, tree );

  if ( !result.err.empty( ) )
  {
    return result;
  }

  result = tree.Write( dvs_ );

  // We are not changing the index so no need to write it here.

  return result;
}

void WriteTreeCommand::ForEachDirectoryInPath( const std::string &                                 path_,
                                               std::function< void( const std::string &newPath ) > func_ )
{
  std::filesystem::path path = path_;

  for ( const auto &newPath : path )
  {
    func_( newPath.string( ) );
  }
}

OidResult WriteTreeCommand::ProcessTreeDirectory( DVS &dvs_, TreeRecord &tree_ )
{
  OidResult result;

  // clang-format off
  tree_.ForAllEntries( [ &dvs_, &result, this ] ( DirEntry &entry_ )
  {
    if ( entry_.type == RecordType::tree )
    {
      ProcessTreeDirectory( dvs_, *entry_.m_Tree );
      result = entry_.m_Tree->Write( dvs_ );

      entry_.oid = result.oid;
    }
  } );
  // clang-format on

  return result;
}