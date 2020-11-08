#pragma once

#include <filesystem>
#include <string>

#include "common.h"

class DVS;

class ReadTreeCommand
{
  public:
  using GetTreeResult = struct
  {
    std::string err;
    std::filesystem::path path;
    std::string oid;
  };

  std::string ParseArgs( std::map< std::string, docopt::value > & );

  std::string operator( )( DVS & );

  OidResult ReadTree( DVS &, const std::string &hashId );

  GetTreeResult GetTree( const std::filesystem::path, const std::string &hashId );

  protected:
  private:
  std::string m_HashId;

  void EmptyCurrentDirectory( DVS & );
};
