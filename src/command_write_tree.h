#pragma once

#include <filesystem>
#include <string>


class DVS;

class WriteTreeCommand
{
  public:

    std::string operator ( ) ( DVS & );

  protected:

  private:

    std::string WriteTree( const std::string &dir = "." );
    bool IsIgnored( const std::filesystem::path & );
};
