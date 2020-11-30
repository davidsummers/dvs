#pragma once

#include <functional>
#include <map>
#include <string>

#include "common.h"

class DVS;

class Index
{
  public:
  using IndexEntry = struct IndexEntry
  {
    std::string filename;
    Oid oid;
  };

  void AddEntry( const std::string &filename, const Oid & );

  void ForAllEntries( std::function< void ( const IndexEntry & ) > );

  Error Read( DVS & );
  Error Write( DVS & );

  protected:
  private:

  using IndexMap = std::map< std::string, IndexEntry >;
  IndexMap m_IndexMap;
};
