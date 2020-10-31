#pragma once

#include <string>

#include "common.h"
#include "docopt.h"


class DVS;

class HashCommand
{
  public:

    std::string ParseArgs( std::map< std::string, docopt::value > & );

    std::string operator ( ) ( DVS & );

    OidResult Hash( DVS &, const std::string &filename, const RecordType type = RecordType::blob );

    OidResult Hash( DVS &, std::istream &, size_t size, const RecordType type = RecordType::blob );

    std::string LookupType( const RecordType );

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
