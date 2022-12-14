#pragma once

#include <map>
#include <string>

#include "command_base.h"
#include "common.h"
#include "docopt.h"

class DVS;

class CreateTagCommand : public BaseCommand
{
  public:
  Error ParseArgs( DocOptArgs & ) override;

  Error operator( )( DVS & ) override;

  Error CreateTag( DVS &, const std::string &tagName, const std::string &hashId = "" );

  protected:
  private:
  std::string m_TagName;
  std::string m_HashId;
};
