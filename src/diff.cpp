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

  CompareTrees( from_, to_, [ &err, &dirPath_, &dvs_ ]( const std::string &path_, const std::vector< TreeRecord::DirEntry > &entries_ )
  {
    if ( entries_.size( ) != 2 || entries_[ 0 ].oid != entries_[ 1 ].oid )
    {
      std::optional< Oid > from;
      std::optional< Oid > to;

      if ( entries_.size( ) >= 1 )
      {
        from = entries_[ 0 ].oid; 
      }

      if ( entries_.size( ) >= 2 )
      {
        to = entries_[ 1 ].oid;
      }

      if ( !to.has_value( ) )
      {
        std::swap( from, to );
      }

      if ( entries_.size( ) >= 1 && entries_[ 0 ].type == RecordType::blob )
      {
        DiffBlob( dvs_, std::cout, from, to, dirPath_, path_ );
      }
      else if ( entries_.size( ) >= 1 && entries_[ 0 ].type == RecordType::tree )
      {
        TreeRecord tree1;
        TreeRecord tree2;

        if ( from.has_value( ) )
        {
          err = tree1.Read( dvs_, from.value( ) );

          if ( !err.empty( ) )
          {
            return;
          }
        }

        if ( to.has_value( ) )
        {
          err = tree2.Read( dvs_, to.value( ) );

          if ( !err.empty( ) )
          {
            return;
          }
        }

        err = DiffTrees( dvs_, tree1, tree2, dirPath_ + "/" + entries_[ 0 ].filename );
      }
    }
  } );

  return err;
}

void Diff::CompareTrees( const TreeRecord &                                                           from_,
                         const TreeRecord &                                                           to_,
                         std::function< void( const std::string &path, const std::vector< TreeRecord::DirEntry > & ) > func_ )
{
  using Path        = std::string;
  using TreePathMap = std::map< Path, std::vector< TreeRecord::DirEntry > >;
  TreePathMap treePathMap;

  from_.ForAllEntries( [ &treePathMap ]( const TreeRecord::DirEntry &entry_ )
  {
    treePathMap[ entry_.filename ] = { entry_ };
  } );

  to_.ForAllEntries( [ &treePathMap ]( const TreeRecord::DirEntry &entry_ )
  {
    TreePathMap::iterator itr = treePathMap.find( entry_.filename );

    if ( itr == treePathMap.end( ) )
    {
      treePathMap[ entry_.filename ] = { entry_ };
    }
    else
    {
      itr->second.push_back( entry_ );
    }
  } );

  for ( const auto &entry : treePathMap )
  {
    func_( entry.first, entry.second );
  }
}

void Diff::DiffBlob( DVS &dvs_, std::ostream &output_, const std::optional< Oid > &from_, const std::optional< Oid > &to_, const std::string &dir_, const std::string &path_ )
{
  output_ << "Index: " << ( from_.has_value( ) ? from_.value( ) : "00000000" ) << "..." << ( to_.has_value( ) ? to_.value( ) : "00000000" ) << std::endl;
  using namespace std;
  void unifiedDiff( const std::string &diffFileName1,
                    const std::string &actualFilename1,
                    std::ifstream &str1,
                    const std::string &diffFilename2,
                    const std::string &actualFilename2,
                    std::ifstream &str2 );
  std::string   diffFilename1 = "a" + std::string( dir_.empty( ) ? "/" : dir_ + "/" ) + path_;
  std::string   diffFilename2 = "b" + std::string( dir_.empty( ) ? "/" : dir_ + "/" ) + path_;
  std::string   actualFilename1;
  std::string   actualFilename2;
  std::ifstream stream1;
  std::ifstream stream2;

  if ( CatCommand::CatResult result = GetStreamFromOid( dvs_, from_, actualFilename1, stream1 ); !result.err.empty( ) && from_.has_value( ) )
  {
    return;
  }

  if ( CatCommand::CatResult result = GetStreamFromOid( dvs_, to_,   actualFilename2, stream2 ); !result.err.empty( ) && to_.has_value( ) )
  {
    return;
  }

  unifiedDiff( diffFilename1, actualFilename1, stream1, diffFilename2, actualFilename2, stream2 );
}


CatCommand::CatResult Diff::GetStreamFromOid( DVS &dvs_, const std::optional< Oid > &oid_, std::string &filename_, std::ifstream &inputStream_ )
{
  CatCommand::CatResult result;

  if ( !oid_.has_value( ) )
  {
    result.err = "Oid has no value.";
    return result;
  }

  std::filesystem::path hashPath = dvs_.GetDvsDirectory( ) / "objects" / oid_.value( ).substr( 0, 2 ) / oid_.value( ).substr( 2 );

  filename_ = hashPath.string( );

  if ( !std::filesystem::exists( filename_ ) )
  {
    std::stringstream ss;
    ss << "Hash " << oid_.value( ) << " does not exist.";
    result.err = ss.str( );
    return result;
  }

  inputStream_.open( filename_, std::ios_base::binary );

  if ( !inputStream_.is_open( ) )
  {
    std::stringstream ss;
    ss << "Can't open file " << filename_ << ".";
    result.err = ss.str( );
    return result;
  }

  std::string header;

  // Read header.
  std::getline( inputStream_, header, '\0' );

  {
    std::string::size_type pos = header.find( ' ' );
    std::string            sizeStr;

    if ( pos != std::string::npos )
    {
      sizeStr = header.substr( pos + 1 );
      header  = header.substr( 0, pos );
    }

    result.size = atoi( sizeStr.c_str( ) );
    result.type = header;
  }

  return result;
}
