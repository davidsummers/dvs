#pragma once

#include <map>
#include <string>

#include "common.h"
#include "docopt.h"

class DVS;

class ListBranchCommand
{
  public:
  Error ParseArgs( std::map< std::string, docopt::value > & );

  Error operator( )( DVS & );

  Error ListBranch( DVS & );

  protected:
  private:
};
