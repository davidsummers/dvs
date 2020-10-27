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

    using HashResult = struct
    {
      std::string err;
      std::string oid;
    };

    std::string ParseArgs( std::map< std::string, docopt::value > & );

    std::string operator ( ) ( DVS & );

    HashResult Hash( DVS &, const std::string &filename, const HashType type = HashType::blob );

    HashResult Hash( DVS &, std::istream &, size_t size, const HashType type = HashType::blob );

    std::string LookupType( const HashType );

  protected:

  private:

    //
    // Data
    //

    std::string m_Filename;

    //
    // Methods
    //
};
