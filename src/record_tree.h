#pragma once

//
// Treet Record
//

#include <istream>
#include <ostream>
#include <string>

#include "common.h"

class TreeRecord
{
  public:
  void AddEntry( const std::string filename_, const RecordType &, std::string &hash );

  std::string Parse( std::istream & );

  std::ostream &operator<<( std::ostream & ) const;

  protected:
  private:
  using DirEntry = struct
  {
    std::string oid;
    RecordType type;
    std::string filename;
  };

  using DirList = std::map< std::string, DirEntry >;

  DirList m_DirList;
};

std::ostream &operator<<( std::ostream &, const TreeRecord & );
