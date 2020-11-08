#pragma once

#include <map>
#include <string>

#include "docopt.h"

class DVS;

class LogCommand
{
  public:
  std::string ParseArgs( std::map< std::string, docopt::value > & );

  std::string operator( )( DVS & );

  std::string GetLog( DVS &, const std::string &hashID );

  protected:
  private:
  std::string m_HashId;
};
