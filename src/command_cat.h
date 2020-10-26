#pragma once

#include <map>
#include <string>

#include "docopt.h"

class DVS;

class CatCommand
{
  public:

    enum class PrintType
    {
      hash,
      type,
    };

    std::string ParseArgs( std::map< std::string, docopt::value > & );

    std::string operator ( ) ( DVS & );

  protected:

  private:

    PrintType   m_PrintType = PrintType::hash;
    std::string m_HashId;
};
