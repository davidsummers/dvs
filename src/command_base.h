#pragma once

#include <map>
#include <string>

#include "common.h"
#include "docopt.h"

class DVS;

class BaseCommand
{
  public:
  BaseCommand( ) = default;
  virtual ~BaseCommand( ) = default;

  virtual Error ParseArgs( DocOptArgs & ) = 0;

  virtual Error operator( )( DVS & ) = 0;

  protected:
  private:
};
