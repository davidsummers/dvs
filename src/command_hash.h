#pragma once

#include <istream>
#include <string>

class DVS;

class HashCommand
{
  public:

    std::string operator ( ) ( DVS &, std::istream &, bool write );

  protected:

  private:

    //
    // Data
    //

    //
    // Methods
    //
};
