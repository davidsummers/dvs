#pragma once

#include <string>

#include "command_base.h"
#include "common.h"
#include "docopt.h"

class DVS;

class HashCommand : public BaseCommand
{
  public:
  Error ParseArgs( DocOptArgs & ) override;

  Error operator( )( DVS & ) override;

  OidResult Hash( DVS &, const std::string &filename, const RecordType type = RecordType::blob );

  OidResult Hash( DVS &, std::istream &, size_t size, const RecordType type = RecordType::blob );

  static std::string LookupType( const RecordType );

  static RecordType LookupType( const std::string &type );

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
