#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "command_cat.h"
#include "diff.h"
#include "dvs.h"
#include "record_tree.h"

std::string Diff::DiffTrees( DVS &dvs_, const TreeRecord &from_, const TreeRecord &to_ )
{
  CompareTrees( from_, to_, [ &dvs_ ]( const std::string &path_, const std::vector< Oid > &oids_ )
  {
    if ( oids_.size( ) != 2 || oids_[ 0 ] != oids_[ 1 ] )
    {
      DiffBlob( dvs_, std::cout, oids_[ 0 ], oids_[ 1 ], path_ );
    }
  } );

  return "";
}

void Diff::CompareTrees( const TreeRecord &                                                           from_,
                         const TreeRecord &                                                           to_,
                         std::function< void( const std::string &path, const std::vector< Oid > & ) > func_ )
{
  using Path        = std::string;
  using TreePathMap = std::map< Path, std::vector< Oid > >;
  TreePathMap treePathMap;

  from_.ForAllEntries( [ &treePathMap ]( const RecordType &type_, const Oid &oid_, const std::string &filename_ )
  {
    treePathMap[ filename_ ] = { oid_ };
  } );

  to_.ForAllEntries( [ &treePathMap ]( const RecordType &type_, const Oid &oid_, const std::string &filename_ )
  {
    TreePathMap::iterator itr = treePathMap.find( filename_ );

    if ( itr == treePathMap.end( ) )
    {
      treePathMap[ filename_ ] = { oid_ };
    }
    else
    {
      itr->second.push_back( oid_ );
    }
  } );

  for ( const auto &entry : treePathMap )
  {
    func_( entry.first, entry.second );
  }
}

void Diff::DiffBlob( DVS &dvs_, std::ostream &output_, const Oid &from_, const Oid &to_, const std::string path_ )
{
  output_ << "Index: " << from_ << "..." << to_ << std::endl;
  using namespace std;
  void unifiedDiff( const std::string &diffFileName1,
                    const std::string &actualFilename1,
                    std::ifstream &str1,
                    const std::string &diffFilename2,
                    const std::string &actualFilename2,
                    std::ifstream &str2 );
  std::string   diffFilename1 = "a/" + path_;
  std::string   diffFilename2 = "b/" + path_;
  std::string   actualFilename1;
  std::string   actualFilename2;
  std::ifstream stream1;
  std::ifstream stream2;

  if ( CatCommand::CatResult result = GetStreamFromOid( dvs_, from_, actualFilename1, stream1 ); !result.err.empty( ) )
  {
    return;
  }

  if ( CatCommand::CatResult result = GetStreamFromOid( dvs_, to_,   actualFilename2, stream2 ); !result.err.empty( ) )
  {
    return;
  }

  unifiedDiff( diffFilename1, actualFilename1, stream1, diffFilename2, actualFilename2, stream2 );
}


CatCommand::CatResult Diff::GetStreamFromOid( DVS &dvs_, const Oid &oid_, std::string &filename_, std::ifstream &inputStream_ )
{
  CatCommand::CatResult result;
  std::filesystem::path hashPath = dvs_.GetDvsDirectory( ) / "objects" / oid_.substr( 0, 2 ) / oid_.substr( 2 );

  filename_ = hashPath.string( );

  if ( !std::filesystem::exists( filename_ ) )
  {
    std::stringstream ss;
    ss << "Hash " << oid_ << " does not exist.";
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
