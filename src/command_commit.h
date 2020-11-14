#pragma once

#include <map>
#include <string>

#include "common.h"
#include "docopt.h"

class DVS;

class CommitCommand
{
  public:
  Error ParseArgs( std::map< std::string, docopt::value > & );

  Error operator( )( DVS & );

  OidResult Commit( DVS &, const std::string &message );

  protected:
  private:
  std::string m_Msg;
};
