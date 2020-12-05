#pragma once

#include <string>

#include "command_base.h"
#include "common.h"

class DVS;
class Index;

class StatusCommand : public BaseCommand
{
  public:
  Error ParseArgs( DocOptArgs & ) override;

  Error operator( )( DVS & ) override;

  Error Status( DVS &, const std::string &path = "" );

  protected:
  private:
  std::string GetBranchName( DVS & );
  Error ShowBranch( DVS &dvs_ );
  OidResult DiffHeadToIndex( DVS &dvs_, const Oid &indexOid );
  OidResult DiffIndexToCurrentDirectory( DVS &, const Oid &indexOid );
};
