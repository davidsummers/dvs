#pragma once

#include <map>
#include <string>

#include "command_base.h"
#include "common.h"
#include "docopt.h"

class DVS;

class DiffCommand : public BaseCommand
{
  public:
  Error ParseArgs( DocOptArgs & ) override;

  Error operator( )( DVS & ) override;

  Error Diff( DVS &, const std::string &path = "", const bool &indexOption = false );

  protected:
  private:

  //
  // Data
  //
  bool m_IndexOption = false;
  std::string m_Path;

  //
  // Methods
  //
  Error DiffIndex( DVS &, const std::string &path = "" );
  Error DiffWorkingCopy( DVS &, const std::string &path = "" );
};
