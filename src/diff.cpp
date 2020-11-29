#include <assert.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "command_cat.h"
#include "diff.h"
#include "dvs.h"

Error Diff::DiffTrees( DVS &dvs_, const TreeRecord &from_, const TreeRecord &to_, const std::string dirPath_ )
{
  Error err;

  // clang-format off
  CompareTrees( from_,
                to_,
                [ &err, &dirPath_, &dvs_ ] ( const std::string &path_,
                                             const TreeRecord::DirEntry &fromEntry_,
                                             const TreeRecord::DirEntry &toEntry_ )
  {
  //clang-format on
    if ( fromEntry_.oid != toEntry_.oid )
    {
      RecordType type = fromEntry_.type == RecordType::none ? toEntry_.type : fromEntry_.type;

      if ( type == RecordType::blob )
      {
        DiffBlob( dvs_, std::cout, fromEntry_.oid, toEntry_.oid, dirPath_, path_ );
      }
      else if ( type == RecordType::tree )
      {
        TreeRecord tree1;
        TreeRecord tree2;

        if ( !fromEntry_.oid.empty( ) )
        {
          err = tree1.Read( dvs_, fromEntry_.oid );

          if ( !err.empty( ) )
          {
            return;
          }
        }

        if ( !toEntry_.oid.empty( ) )
        {
          err = tree2.Read( dvs_, toEntry_.oid );

          if ( !err.empty( ) )
          {
            return;
          }
        }

        err = DiffTrees( dvs_, tree1, tree2, dirPath_ + "/" + fromEntry_.filename );
      }
    }
  } );

  return err;
}

Error Diff::ListChangedFiles( DVS &dvs_,
                              const TreeRecord &from_,
                              const TreeRecord &to_,
                              const std::string dirPath_ )
{
  Error err;

  CompareTrees( from_,
                to_,
                [ &err, &dirPath_, &dvs_ ] ( const std::string &path_, const TreeRecord::DirEntry &fromEntry_, const TreeRecord::DirEntry &toEntry_ )
  {
      if ( fromEntry_.oid != toEntry_.oid )
      {
        RecordType  type     = fromEntry_.type == RecordType::none ? toEntry_.type : fromEntry_.type;
        std::string filename = fromEntry_.filename.empty( ) ? toEntry_.filename : fromEntry_.filename;

        if ( type == RecordType::blob )
        {
          std::string diffFilename = std::string( dirPath_.empty( ) ? "/" : dirPath_ + "/" ) + path_ + filename;
          std::cout << diffFilename << std::endl;
        }
        else if ( type == RecordType::tree )
        {
          TreeRecord tree1;
          TreeRecord tree2;

          if ( !fromEntry_.oid.empty( ) )
          {
            err = tree1.Read( dvs_, fromEntry_.oid );

            if ( !err.empty( ) )
            {
              return;
            }
          }

          if ( !toEntry_.oid.empty( ) )
          {
            err = tree2.Read( dvs_, toEntry_.oid );

            if ( !err.empty( ) )
            {
              return;
            }
          }

          err = DiffTrees( dvs_, tree1, tree2, dirPath_ + "/" + fromEntry_.filename );
        }
      }
    } );

  return err;
}

void Diff::CompareTrees( const TreeRecord &                                             fromTree_,
                         const TreeRecord &                                             toTree_,
                         std::function< void( const std::string &         path,
                                              const TreeRecord::DirEntry &fromRecord_,
                                              const TreeRecord::DirEntry &toRecord_ ) > func_ )
{
  using Path          = std::string;
  using TreePathEntry = struct TreePathEntry
  {
    TreeRecord::DirEntry from;
    TreeRecord::DirEntry to;
  };

  using TreePathMap = std::map< Path, TreePathEntry >;
  TreePathMap treePathMap;

  fromTree_.ForAllEntries(
    [ &treePathMap ]( const TreeRecord::DirEntry &entry_ )
    {
      TreePathEntry entry;
      entry.from                     = entry_;
      treePathMap[ entry_.filename ] = entry;
    } );

  toTree_.ForAllEntries(
    [ &treePathMap ]( const TreeRecord::DirEntry &entry_ )
    {
      TreePathMap::iterator itr = treePathMap.find( entry_.filename );

      if ( itr == treePathMap.end( ) )
      {
        TreePathEntry entry;
        entry.to                       = entry_;
        treePathMap[ entry_.filename ] = entry;
      }
      else
      {
        TreePathEntry &entry = itr->second;
        entry.to             = entry_;
      }
    } );

  for ( const auto &entry : treePathMap )
  {
    func_( entry.first, entry.second.from, entry.second.to );
  }
}

void Diff::DiffBlob( DVS               &dvs_,
                     std::ostream      &output_,
                     const Oid         &from_,
                     const Oid         &to_,
                     const std::string &dir_,
                     const std::string &path_ )
{
  output_ << "Index: " << ( !from_.empty( ) ? from_ : "00000000" ) << "..." << ( !to_.empty( ) ? to_ : "00000000" )
          << std::endl;
  using namespace std;
  void          unifiedDiff( const std::string &diffFileName1,
                             const std::string &actualFilename1,
                             std::ifstream     &str1,
                             const std::string &diffFilename2,
                             const std::string &actualFilename2,
                             std::ifstream     &str2 );
  std::string   diffFilename1 = "a" + std::string( dir_.empty( ) ? "/" : dir_ + "/" ) + path_;
  std::string   diffFilename2 = "b" + std::string( dir_.empty( ) ? "/" : dir_ + "/" ) + path_;
  std::string   actualFilename1;
  std::string   actualFilename2;
  std::ifstream stream1;
  std::ifstream stream2;

  if ( CatCommand::CatResult result = CatCommand::GetStreamFromOid( dvs_, from_, actualFilename1, stream1 );
       !result.err.empty( ) && !from_.empty( ) )
  {
    return;
  }

  if ( CatCommand::CatResult result = CatCommand::GetStreamFromOid( dvs_, to_, actualFilename2, stream2 );
       !result.err.empty( ) && !to_.empty( ) )
  {
    return;
  }

  unifiedDiff( diffFilename1, actualFilename1, stream1, diffFilename2, actualFilename2, stream2 );
}
