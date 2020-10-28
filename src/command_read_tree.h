#pragma once

#include <filesystem>
#include <string>


class DVS;

class ReadTreeCommand
{
  public:

    using ReadTreeResult = struct
    {
      std::string err;
      std::string oid;
    };

    std::string operator ( ) ( DVS & );

  protected:

  private:

//    WriteTreeResult WriteTree( DVS &, const std::string &dir = "." );
};
