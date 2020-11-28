#pragma once

//
// Tree Record
//

#include <istream>
#include <map>
#include <ostream>
#include <string>

#include "common.h"

class DVS;

class TreeRecord
{
  public:
  using DirEntry = struct DirEntry
  {
    Oid         oid;
    RecordType  type = RecordType::none;
    std::string filename;
  };

  void AddEntry( const std::string filename_, const RecordType &, std::string &hash );

  Error     Read( DVS &, const Oid & );
  OidResult Write( DVS & );

  std::ostream &operator<<( std::ostream & ) const;

  void ForAllEntries( std::function< void( const DirEntry & ) > ) const;

  protected:
  private:
  //
  // Data
  //

  using DirList = std::map< std::string, DirEntry >;

  DirList m_DirList;

  //
  // Methods
  //

  Error Parse( std::istream & );
};

std::ostream &operator<<( std::ostream &, const TreeRecord & );
