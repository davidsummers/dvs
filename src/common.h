#pragma once

//
// Common declarations
//

#include <string>

#include "docopt.h"

using OidResult = struct
{
  std::string err;
  std::string oid;
};
