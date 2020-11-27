#pragma once

#include <string>

#include "command_base.h"
#include "common.h"

class DVS;

class StatusCommand : public BaseCommand
{
  public:
  Error ParseArgs( DocOptArgs & ) override;

  Error operator( )( DVS & ) override;

  Error Status( DVS & );

  protected:
  private:
  std::string GetBranchName( DVS & );
};
