#pragma once

#include <filesystem>
#include <string>


class DVS;

class WriteTreeCommand
{
  public:

    using WriteTreeResult = struct
    {
      std::string err;
      std::string oid;
    };

    std::string operator ( ) ( DVS & );

  protected:

  private:

    WriteTreeResult WriteTree( DVS &, const std::string &dir = "." );
    bool IsIgnored( const std::filesystem::path & );
};
