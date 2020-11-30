#pragma once

#include <map>
#include <string>

#include "command_base.h"
#include "common.h"
#include "docopt.h"

class DVS;

class CommitCommand : public BaseCommand
{
  public:
  Error ParseArgs( DocOptArgs & ) override;

  Error operator( )( DVS & ) override;

  OidResult Commit( DVS &, const std::string &message, const std::vector< std::string > &path = { } );

  protected:
  private:
  std::string m_Msg;
  std::vector< std::string > m_PathList;
};
