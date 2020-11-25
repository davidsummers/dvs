#pragma once

//
// Diff declaration
//

#include <functional>
#include <ostream>

#include "common.h"

class TreeRecord;

class Diff
{
  public:
  static std::string DiffTrees( const TreeRecord &from, const TreeRecord &to );

  protected:
  private:
  static void CompareTrees( const TreeRecord &from,
                            const TreeRecord &to,
                            std::function< void( const std::string &path, const std::vector< Oid > & ) > );
  static void DiffBlob( std::ostream &, const Oid &from, const Oid &to, const std::string path );
};
