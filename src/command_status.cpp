#include <cassert>
#include <iostream>
#include <sstream>

#include "command_status.h"
#include "dvs.h"

Error StatusCommand::operator( )( DVS &dvs_ )
{
  if ( Error validate_error = dvs_.Validate( ); !validate_error.empty( ) )
  {
    std::stringstream ss;
    ss << "Can't validate " << s_DVS_DIR << " directory: " + validate_error;
    return ss.str( );
  }

  Error err = Status( dvs_ );

  return err;
}

Error StatusCommand::Status( DVS &dvs_ )
{
  std::cout << "Status: " << std::endl;

  std::string head = dvs_.GetOid( "@" );

  if ( head.empty( ) )
  {
    return "Can't find head branch.";
  }

  std::string branch = GetBranchName( dvs_ );

  if ( !branch.empty( ) )
  {
    std::cout << "On branch " << branch << std::endl;
  }
  else
  {
    std::cout << "HEAD detached at " << head << std::endl;
  }

  return "";
}

std::string StatusCommand::GetBranchName( DVS &dvs_ )
{
  const bool deref = false;
  RefValue   head  = dvs_.GetRef( s_HEAD_REF, deref );

  if ( !head.symbolic )
  {
    return "";
  }

  assert( ( (void) "Head starts with refs/locals", head.value.find( s_REFS_BRANCHES_LOCAL ) == 0 ) );

  std::string result = head.value.substr( head.value.find( s_REFS_BRANCHES_LOCAL ) );

  return result;
}
