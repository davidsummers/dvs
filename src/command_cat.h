#pragma once

#include <string>

class DVS;

class CatCommand
{
  public:

    std::string operator ( ) ( DVS &, const std::string &hashId );

  protected:

  private:
};
