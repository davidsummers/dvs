#pragma once

#include <string>

class DVS;

class StatusCommand
{
  public:
  std::string operator( )( DVS & );

  std::string Status( DVS & );

  protected:
  private:
  std::string GetBranchName( DVS & );
};
