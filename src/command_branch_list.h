#pragma once

#include <map>
#include <string>

#include "common.h"
#include "docopt.h"

class DVS;

class ListBranchCommand
{
  public:
  std::string ParseArgs( std::map< std::string, docopt::value > & );

  std::string operator( )( DVS & );

  std::string ListBranch( DVS & );

  protected:
  private:
};
