#pragma once

//
// Common declarations
//

#include <string>

#include "docopt.h"

enum class RecordType
{
  none,
  blob,
  commit,
  tree,
  tag,
};

using OidResult = struct
{
  std::string err;
  std::string oid;
};
