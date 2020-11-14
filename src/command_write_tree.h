#pragma once

#include <filesystem>
#include <string>

#include "common.h"

class DVS;

class WriteTreeCommand
{
  public:
  Error operator( )( DVS & );

  OidResult WriteTree( DVS &, const std::string &dir = "." );

  protected:
  private:
};
