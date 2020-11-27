#pragma once

#include <map>
#include <string>

#include "command_base.h"
#include "common.h"
#include "docopt.h"

class DVS;

class DeleteBranchCommand : public BaseCommand
{
  public:
  Error ParseArgs( DocOptArgs & ) override;

  Error operator( )( DVS & ) override;

  Error DeleteBranch( DVS &, const std::string &branchName );

  protected:
  private:
  std::string m_Branch;
};
