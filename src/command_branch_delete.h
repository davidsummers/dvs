#pragma once

#include <map>
#include <string>

#include "common.h"
#include "docopt.h"

class DVS;

class DeleteBranchCommand
{
  public:
  Error ParseArgs( std::map< std::string, docopt::value > & );

  Error operator( )( DVS & );

  Error DeleteBranch( DVS &, const std::string &branchName );

  protected:
  private:
  std::string m_Branch;
};
