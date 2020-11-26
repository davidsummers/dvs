#pragma once

#include <map>
#include <string>

#include "command_base.h"
#include "common.h"
#include "docopt.h"

class DVS;

class CreateBranchCommand : public BaseCommand
{
  public:
  Error ParseArgs( DocOptArgs & );

  Error operator( )( DVS & );

  Error CreateBranch( DVS &, const std::string &branchName, const Oid &oid_ = "@" );

  protected:
  private:
  std::string m_BranchName;
  Oid m_Oid = "@";
};
