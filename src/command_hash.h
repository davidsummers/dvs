#pragma once

#include <istream>
#include <string>

class DVS;

class HashCommand
{
  public:

    enum class HashType
    {
      blob,
      commit,
      tree,
      tag,
    };

    std::string operator ( ) ( DVS &, const HashType, const std::string &filename, std::istream &, bool write );

  protected:

  private:

    //
    // Data
    //

    //
    // Methods
    //
};
