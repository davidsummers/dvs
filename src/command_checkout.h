#pragma once

#include <map>
#include <string>

#include "docopt.h"

class DVS;

class CheckoutCommand
{
  public:
  std::string ParseArgs( std::map< std::string, docopt::value > & );

  std::string operator( )( DVS & );

  std::string Checkout( DVS &, const std::string &name );

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
