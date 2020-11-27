#pragma once

#include <map>
#include <string>

#include "command_base.h"
#include "docopt.h"

class DVS;

class LogCommand : public BaseCommand
{
  public:
  Error ParseArgs( DocOptArgs & ) override;

  Error operator( )( DVS & ) override;

  Error GetLog( DVS &, const std::string &hashID, bool showPatchDiff = false );

  protected:
  private:
  std::string m_HashId;
  bool m_ShowPatchDiff = false;
};
