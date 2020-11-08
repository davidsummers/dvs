#pragma once

#include <filesystem>
#include <string>

#include "common.h"

class DVS;

class WriteTreeCommand
{
  public:
  std::string operator( )( DVS & );

  OidResult WriteTree( DVS &, const std::string &dir = "." );

  protected:
  private:
};
