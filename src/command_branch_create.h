#pragma once

#include <map>
#include <string>

#include "common.h"
#include "docopt.h"

class DVS;

class CreateBranchCommand
{
  public:
  Error ParseArgs( std::map< std::string, docopt::value > & );

  Error operator( )( DVS & );

  Error CreateBranch( DVS &, const std::string &branchName, const std::string &oid_ = "@" );

  protected:
  private:
  std::string m_BranchName;
  std::string m_Oid = "@";
};
