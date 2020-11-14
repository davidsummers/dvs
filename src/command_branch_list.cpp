#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>

#include "command_branch_list.h"
#include "dvs.h"

Error ListBranchCommand::ParseArgs( std::map< std::string, docopt::value > & /* args_ */ )
{
  Error err;

  return err;
}

Error ListBranchCommand::operator( )( DVS &dvs_ )
{
  if ( Error validateError = dvs_.Validate( ); !validateError.empty( ) )
  {
    return validateError;
  }

  return ListBranch( dvs_ );
}

Error ListBranchCommand::ListBranch( DVS &dvs_ )
{
  Error err;

  std::filesystem::path newDir = dvs_.GetDvsDirectory( );
  newDir /= s_REFS_BRANCHES_LOCAL;

  if ( std::filesystem::exists( newDir ) )
  {
    std::cout << "Local Branches:" << std::endl;

    for ( auto const &entry : std::filesystem::directory_iterator( newDir ) )
    {
      if ( entry.is_regular_file( ) )
      {
        std::string branchName = entry.path( ).string( );
        size_t      len        = newDir.native( ).length( );
        branchName             = branchName.substr( len );
        std::cout << branchName << std::endl;
      }
    }

    std::cout << std::endl;
  }

  newDir = dvs_.GetDvsDirectory( );
  newDir /= s_REFS_BRANCHES_REMOTE;

  if ( std::filesystem::exists( newDir ) )
  {
    std::cout << "Remote Branches: " << std::endl;

    for ( auto const &entry : std::filesystem::directory_iterator( newDir ) )
    {
      if ( entry.is_regular_file( ) )
      {
        std::string branchName = entry.path( ).string( );
        size_t      len        = newDir.native( ).length( );
        branchName             = branchName.substr( len );
        std::cout << branchName << std::endl;
      }
    }

    std::cout << std::endl;
  }

  return err;
}
