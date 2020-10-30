#pragma once

#include <string>

#include "common.h"


class DVS;

class CommitCommand
{
  public:

    std::string ParseArgs( std::map< std::string, docopt::value > & );

    std::string operator ( ) ( DVS & );

    OidResult Commit( DVS &, const std::string &message );

  protected:

  private:

    std::string m_Msg;
};
