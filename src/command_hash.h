#pragma once

#include <string>

#include "common.h"
#include "docopt.h"

class DVS;

class HashCommand
{
  public:
  Error ParseArgs( std::map< std::string, docopt::value > & );

  Error operator( )( DVS & );

  OidResult Hash( DVS &, const std::string &filename, const RecordType type = RecordType::blob );

  OidResult Hash( DVS &, std::istream &, size_t size, const RecordType type = RecordType::blob );

  static std::string LookupType( const RecordType );

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
