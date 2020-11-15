#include "diff.h"
#include "record_tree.h"

std::string Diff::DiffTrees( const TreeRecord &from_, const TreeRecord &to_ )
{
  std::string output;

  CompareTrees( from_, to_, [ &output ] ( const std::string &path_, const TreeRecord &ofrom_, const TreeRecord &oto_ )
  {
  } );

  return output;
}


void Diff::CompareTrees( const TreeRecord &from_, const TreeRecord &to_, std::function< void ( const std::string &path, const TreeRecord &from, const TreeRecord &to ) > func_ )
{
  
}
