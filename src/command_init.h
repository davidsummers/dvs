#pragma once

#include <map>
#include <string>

#include "docopt.h"

class DVS;

class InitCommand
{
  public:

    std::string ParseArgs( std::map< std::string, docopt::value > & );

    std::string operator ( ) ( DVS & );

    std::string InitDvs( DVS &, const std::string rootPath = "" );

  protected:

  private:

    std::string m_Directory;
};
