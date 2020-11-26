#pragma once

#include <map>
#include <string>

#include "common.h"
#include "docopt.h"

class DVS;

class TagCommand
{
  public:
  Error ParseArgs( DocOptArgs & );

  Error operator( )( DVS & );

  Error Tag( DVS &, const std::string &tagName, const std::string &hashId = "" );

  protected:
  private:
  std::string m_TagName;
  std::string m_HashId;
};
