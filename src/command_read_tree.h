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

    std::string ParseArgs( std::map< std::string, docopt::value > & );

    std::string operator ( ) ( DVS & );

    ReadTreeResult ReadTree( DVS &, const std::string &hashId );

  protected:

  private:

    std::string m_HashId;
};
