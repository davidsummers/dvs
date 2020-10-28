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
      contents,
      type,
    };

    std::string ParseArgs( std::map< std::string, docopt::value > & );

    std::string operator ( ) ( DVS & );

    std::string GetHash( DVS &, const std::string &hashID );

  protected:

  private:

    PrintType   m_PrintType = PrintType::contents;
    std::string m_HashId;
};
