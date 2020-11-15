#pragma once

//
// Diff declaration
//

#include <functional>

#include "common.h"

class TreeRecord;

class Diff
{
  public:
  static std::string DiffTrees( const TreeRecord &from, const TreeRecord &to );

  protected:
  private:
  static void CompareTrees( const TreeRecord &from, const TreeRecord &to, std::function< void ( const std::string &path, const TreeRecord &from, const TreeRecord &to ) > );
};
