#pragma once

#include <map>
#include <string>

#include "common.h"
#include "docopt.h"

class DVS;

class InitCommand
{
  public:
  Error ParseArgs( std::map< std::string, docopt::value > & );

  Error operator( )( DVS & );

  Error InitDvs( DVS &, const std::string rootPath = "", std::ostream *outStream = nullptr );

  protected:
  private:
  std::string m_Directory;
};
