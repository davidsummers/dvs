#pragma once

#include <map>
#include <string>

#include "command_base.h"
#include "common.h"
#include "docopt.h"

class DVS;

class UnimplementedCommand : public BaseCommand
{
  public:
  UnimplementedCommand( ) = default;
  virtual ~UnimplementedCommand( ) = default;

  virtual Error ParseArgs( DocOptArgs & ) override;

  virtual Error operator( )( DVS & ) override;

  protected:
  private:
};
