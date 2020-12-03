#pragma once

#include <filesystem>
#include <string>

#include "command_base.h"
#include "common.h"
#include "docopt.h"

class DVS;

class ReadTreeCommand : public BaseCommand
{
  public:
  using GetTreeResult = struct
  {
    Error                 err;
    std::filesystem::path path;
    Oid                   oid;
  };

  Error ParseArgs( DocOptArgs & ) override;

  Error operator( )( DVS & ) override;

  OidResult ReadTreeToDirectory( DVS &, const std::string &hashId );

  protected:
  private:
  std::string m_HashId;

  void EmptyCurrentDirectory( DVS & );
};
