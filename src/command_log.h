#pragma once

#include <map>
#include <string>

#include "docopt.h"

class DVS;

class LogCommand
{
  public:
  Error ParseArgs( std::map< std::string, docopt::value > & );

  Error operator( )( DVS & );

  Error GetLog( DVS &, const std::string &hashID, bool showPatchDiff = false );

  protected:
  private:
  std::string m_HashId;
  bool m_ShowPatchDiff = false;
};
