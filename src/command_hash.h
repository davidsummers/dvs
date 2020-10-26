#pragma once

#include <string>

#include "docopt.h"


class DVS;

class HashCommand
{
  public:

    enum class HashType
    {
      none,
      blob,
      commit,
      tree,
      tag,
    };

    std::string ParseArgs( std::map< std::string, docopt::value > & );

    std::string operator ( ) ( DVS & );

  protected:

  private:

    //
    // Data
    //

    HashType    m_HashType  = HashType::blob;
    bool        m_StdInput  = false;
    bool        m_WriteFile = false;
    std::string m_Filename;

    //
    // Methods
    //
};
