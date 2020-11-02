#pragma once

#include <map>
#include <string>

#include "common.h"
#include "docopt.h"

class DVS;

class CreateBranchCommand
{
  public:

    std::string ParseArgs( std::map< std::string, docopt::value > & );

    std::string operator ( ) ( DVS & );

    std::string Branch( DVS &, const std::string &branchName );

  protected:

  private:
};
