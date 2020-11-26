#pragma once

#include <map>
#include <optional>
#include <string>

#include "command_base.h"
#include "common.h"
#include "docopt.h"

class DVS;

class CatCommand : public BaseCommand
{
  public:
  enum class PrintType
  {
    contents,
    size,
    type,
  };

  using CatResult = struct
  {
    Error       err;
    size_t      size;
    std::string type;
  };

  Error ParseArgs( DocOptArgs & );

  Error operator( )( DVS & );

  CatResult GetHash( DVS &,
                     const std::string &hashID,
                     std::ostream *,
                     RecordType expectedRecordType = RecordType::none );

  static CatCommand::CatResult GetStreamFromOid( DVS &, const std::optional< Oid > &, std::string &filename, std::ifstream & );

  protected:
  private:
  PrintType   m_PrintType = PrintType::contents;
  std::string m_HashId;
};
