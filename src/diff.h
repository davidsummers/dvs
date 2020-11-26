#pragma once

//
// Diff declaration
//

#include <functional>
#include <optional>
#include <ostream>

#include "command_cat.h"
#include "common.h"
#include "record_tree.h"

class DVS;
class TreeRecord;

class Diff
{
  public:
  static std::string DiffTrees( DVS &, const TreeRecord &from, const TreeRecord &to, const std::string dirPath = "" );

  protected:
  private:
  static void CompareTrees( const TreeRecord &from,
                            const TreeRecord &to,
                            std::function< void( const std::string &path, const std::vector< TreeRecord::DirEntry > & ) > );
  static void DiffBlob( DVS &, std::ostream &, const std::optional< Oid > &from, const std::optional< Oid > &to, const std::string &dir, const std::string &path );

  static CatCommand::CatResult GetStreamFromOid( DVS &, const std::optional< Oid > &, std::string &filename, std::ifstream & );
};
