#pragma once

#include <string>

class DVS;

class CatCommand
{
  public:

    enum class PrintType
    {
      hash,
      type,
    };

    std::string operator ( ) ( DVS &, const PrintType, const std::string &hashId );

  protected:

  private:
};
