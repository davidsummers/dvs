#pragma once

#include <string>

#include "command_base.h"
#include "common.h"

class DVS;

class StatusCommand : public BaseCommand
{
  public:
  Error ParseArgs( DocOptArgs & );

  Error operator( )( DVS & );

  Error Status( DVS & );

  protected:
  private:
  std::string GetBranchName( DVS & );
};
