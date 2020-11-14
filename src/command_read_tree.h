#pragma once

#include <filesystem>
#include <string>

#include "common.h"
#include "docopt.h"

class DVS;

class ReadTreeCommand
{
  public:
  using GetTreeResult = struct
  {
    Error                 err;
    std::filesystem::path path;
    Oid                   oid;
  };

  Error ParseArgs( std::map< std::string, docopt::value > & );

  Error operator( )( DVS & );

  OidResult ReadTree( DVS &, const std::string &hashId );

  GetTreeResult GetTree( const std::filesystem::path, const std::string &hashId );

  protected:
  private:
  std::string m_HashId;

  void EmptyCurrentDirectory( DVS & );
};
