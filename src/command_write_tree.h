#pragma once

#include <filesystem>
#include <string>

#include "command_base.h"
#include "common.h"

class DVS;
class TreeRecord;

class WriteTreeCommand : public BaseCommand
{
  public:
  Error ParseArgs( DocOptArgs & ) override;

  Error operator( )( DVS & ) override;

  OidResult WriteTreeFromDirectory( DVS &, const std::string &dir = "." );
  OidResult WriteTreeFromIndex( DVS &, const std::string &dir = "." );

  protected:
  private:
  void ForEachDirectoryInPath( const std::string &path, std::function< void ( const std::string &newPath ) > );
  OidResult ProcessTreeDirectory( DVS &, TreeRecord & );
};
