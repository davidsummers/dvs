#pragma once

#include <filesystem>
#include <string>

#include "common.h"

class DVS;

class ReadTreeGenerator
{
  public:
  using GetTreeResult = struct
  {
    std::string err;
    std::filesystem::path path;
    std::string oid;
  };

  GetTreeResult GetTree( const std::filesystem::path, const std::string &hashId );

  protected:
  private:
};
