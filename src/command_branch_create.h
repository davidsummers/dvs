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

    std::string CreateBranch( DVS &, const std::string &branchName, const std::string &oid_ = "@" );

  protected:

  private:

    std::string m_BranchName;
    std::string m_Oid = "@";
};
