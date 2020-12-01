#pragma once

#include <map>
#include <string>

#include "command_base.h"
#include "common.h"
#include "docopt.h"

class DVS;

class IndexAddCommand : public BaseCommand
{
  public:
  Error ParseArgs( DocOptArgs & ) override;

  Error operator( )( DVS & ) override;

  Error IndexAdd( DVS &, const std::string &pathName );

  protected:
  private:
    std::string m_Pathname;
};
