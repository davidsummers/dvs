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
  static Error DiffTrees( DVS &, const TreeRecord &from, const TreeRecord &to, const std::string dirPath = "" );

  static Error ListChangedFiles( DVS &, const TreeRecord &from, const TreeRecord &to, const std::string dirPath = "" );

  protected:
  private:
  static void CompareTrees(
    const TreeRecord &from,
    const TreeRecord &to,
    std::function<
      void( const std::string &path,
            const TreeRecord::DirEntry &fromRecord,
            const TreeRecord::DirEntry &toRecord ) > );

  static void DiffBlob( DVS &,
                        std::ostream &,
                        const Oid         &from,
                        const Oid         &to,
                        const std::string &dir,
                        const std::string &path );
};
