#pragma once

#include <map>
#include <string>

#include "common.h"
#include "docopt.h"

class DVS;

class ListBranchCommand
{
  public:
  Error ParseArgs( DocOptArgs & );

  Error operator( )( DVS & );

  Error ListBranch( DVS & );

  protected:
  private:
};
