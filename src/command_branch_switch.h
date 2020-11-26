#pragma once

#include <map>
#include <string>

#include "docopt.h"

class DVS;

class SwitchBranchCommand : public BaseCommand
{
  public:
  Error ParseArgs( DocOptArgs & );

  Error operator( )( DVS & );

  Error Switch( DVS &, const std::string &name );

  protected:
  private:
  //
  // Data
  //

  std::string m_BranchName;

  //
  // Methods
  //

  bool IsBranch( DVS &, const std::string &branchName );
};
