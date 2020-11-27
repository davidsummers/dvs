#pragma once

#include <map>
#include <string>

#include "command_base.h"
#include "common.h"
#include "docopt.h"

class DVS;

class ListBranchCommand : public BaseCommand
{
  public:
  Error ParseArgs( DocOptArgs & ) override;

  Error operator( )( DVS & ) override;

  Error ListBranch( DVS & );

  protected:
  private:
};
