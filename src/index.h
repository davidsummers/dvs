#pragma once

#include <functional>
#include <map>
#include <string>

#include "common.h"

class DVS;

class Index
{
  public:

  Error AddEntry( DVS &, const std::string &filename );
  void AddEntry( DVS &, const DirEntry & );
  Error RemoveEntry( DVS &, const std::string &filename );

  void ForAllEntries( std::function< void ( const DirEntry & ) > );

  Error Read( DVS & );
  Error Write( DVS & );

  // Read, perform function, then Write index.
  Error WithIndex( DVS &, std::function< Error ( ) > );

  protected:
  private:

  using IndexMap = std::map< std::string, DirEntry >;
  IndexMap m_IndexMap;
  bool m_Read = false; // True if the index has been read from disk.
  bool m_Dirty = false; // True if the index has changed and needs to be written out to disk.
};
