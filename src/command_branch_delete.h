#pragma once

#include <map>
#include <string>

#include "common.h"
#include "docopt.h"

class DVS;

class DeleteBranchCommand
{
  public:
  std::string ParseArgs( std::map< std::string, docopt::value > & );

  std::string operator( )( DVS & );

  std::string DeleteBranch( DVS &, const std::string &branchName );

  protected:
  private:
  std::string m_Branch;
};
