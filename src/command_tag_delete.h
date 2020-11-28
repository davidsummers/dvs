#pragma once

#include <map>
#include <string>

#include "command_base.h"
#include "common.h"
#include "docopt.h"

class DVS;

class DeleteTagCommand : public BaseCommand
{
  public:
  Error ParseArgs( DocOptArgs & ) override;

  Error operator( )( DVS & ) override;

  Error DeleteTag( DVS &, const std::string &tagName );

  protected:
  private:
  std::string m_TagName;
};
