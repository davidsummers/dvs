#pragma once

#include <map>
#include <string>

#include "common.h"
#include "docopt.h"

class DVS;

class CatCommand
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
    std::string err;
    size_t size;
    std::string type;
  };

  std::string ParseArgs( std::map< std::string, docopt::value > & );

  std::string operator( )( DVS & );

  CatResult GetHash(
    DVS &,
    const std::string &hashID,
    std::ostream *,
    RecordType expectedRecordType = RecordType::none );

  protected:
  private:
  PrintType m_PrintType = PrintType::contents;
  std::string m_HashId;
};
