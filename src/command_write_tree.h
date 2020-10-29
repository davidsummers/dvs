#pragma once

#include <filesystem>
#include <string>

#include "common.h"


class DVS;

class WriteTreeCommand
{
  public:

    std::string operator ( ) ( DVS & );

  protected:

  private:

    OidResult WriteTree( DVS &, const std::string &dir = "." );
    bool IsIgnored( const std::filesystem::path & );
};
