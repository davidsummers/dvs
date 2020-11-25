#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "diff.h"
#include "record_tree.h"

std::string Diff::DiffTrees( const TreeRecord &from_, const TreeRecord &to_ )
{
  std::stringstream output;

  CompareTrees( from_, to_, [ &output ]( const std::string &path_, const std::vector< Oid > &oids_ ) {
    if ( oids_.size( ) != 2 || oids_[ 0 ] != oids_[ 1 ] )
    {
      DiffBlob( output, oids_[ 0 ], oids_[ 1 ], path_ );
    }
  } );

  return output.str( );
}

void Diff::CompareTrees( const TreeRecord &                                                           from_,
                         const TreeRecord &                                                           to_,
                         std::function< void( const std::string &path, const std::vector< Oid > & ) > func_ )
{
  using Path        = std::string;
  using TreePathMap = std::map< Path, std::vector< Oid > >;
  TreePathMap treePathMap;

  from_.ForAllEntries( [ &treePathMap ]( const RecordType &type_, const Oid &oid_, const std::string &filename_ ) {
    treePathMap[ filename_ ] = { oid_ };
  } );

  to_.ForAllEntries( [ &treePathMap ]( const RecordType &type_, const Oid &oid_, const std::string &filename_ ) {
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

void Diff::DiffBlob( std::ostream &output_, const Oid &from_, const Oid &to_, const std::string path_ )
{
  output_ << "Changed: " << path_ << std::endl;
  using namespace std;
  void        unifiedDiff( string fp1, string fp2 );
  std::string fp1 = from_;
  std::string fp2 = to_;
  unifiedDiff( fp1, fp2 );
}
