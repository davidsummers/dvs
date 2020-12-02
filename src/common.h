#pragma once

//
// Common declarations
//

#include <string>

#include "docopt.h"

using DocOptArgs = std::map< std::string, docopt::value >;
using Error = std::string;
using Oid   = std::string;

enum class RecordType
{
  none,
  blob,
  commit,
  tree,
  tag,
};

using OidResult = struct OidResult
{
  Error err;
  Oid   oid;
};

using RefValue = struct RefValue
{
  bool        symbolic = false;
  std::string value;
};

class TreeRecord;

class DirEntry
{
  public:
  ~DirEntry( );

  Oid         oid;
  RecordType  type = RecordType::none;
  std::string filename;
  TreeRecord *m_Tree = nullptr;
};
