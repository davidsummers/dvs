#pragma once

#include <filesystem>
#include <string>

#include "command_base.h"
#include "common.h"

class DVS;

class WriteTreeCommand : public BaseCommand
{
  public:
  Error ParseArgs( DocOptArgs & );

  Error operator( )( DVS & );

  OidResult WriteTree( DVS &, const std::string &dir = "." );

  protected:
  private:
};
