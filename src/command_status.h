#pragma once

#include <string>

#include "common.h"

class DVS;

class StatusCommand
{
  public:
  Error operator( )( DVS & );

  Error Status( DVS & );

  protected:
  private:
  std::string GetBranchName( DVS & );
};
