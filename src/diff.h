#pragma once

//
// Diff declaration
//

#include <functional>
#include <ostream>

#include "command_cat.h"
#include "common.h"

class DVS;
class TreeRecord;

class Diff
{
  public:
  static std::string DiffTrees( DVS &, const TreeRecord &from, const TreeRecord &to );

  protected:
  private:
  static void CompareTrees( const TreeRecord &from,
                            const TreeRecord &to,
                            std::function< void( const std::string &path, const std::vector< Oid > & ) > );
  static void DiffBlob( DVS &, std::ostream &, const Oid &from, const Oid &to, const std::string path );

  static CatCommand::CatResult GetStreamFromOid( DVS &, const Oid &, std::string &filename, std::ifstream & );
};
