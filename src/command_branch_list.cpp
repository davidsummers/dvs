#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>

#include "command_branch_list.h"
#include "dvs.h"


std::string ListBranchCommand::ParseArgs( std::map< std::string, docopt::value > & /* args_ */ )
{
  std::string err;

  return err;
}


std::string ListBranchCommand::operator ( ) ( DVS &dvs_ )
{
  if ( std::string validateError = dvs_.Validate( );
       !validateError.empty( ) )
  {
    return validateError;
  }
  
  return ListBranch( dvs_ );
}


std::string ListBranchCommand::ListBranch( DVS &dvs_ )
{
  std::string result;

  std::filesystem::path newDir = dvs_.GetDvsDirectory( );
  newDir /= s_REFS_BRANCHES_LOCAL;

  if ( std::filesystem::exists( newDir ) )
  {
    std::cout << "Local Branches:" << std::endl;

    for ( auto const &entry : std::filesystem::directory_iterator( newDir ) )
    {
      if ( entry.is_regular_file( ) )
      {
        std::string branchName = entry.path( );
        size_t len = newDir.native( ).length( );
        branchName = branchName.substr( len );
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
        std::string branchName = entry.path( );
        size_t len = newDir.native( ).length( );
        branchName = branchName.substr( len );
        std::cout << branchName << std::endl;
      }
    }

    std::cout << std::endl;
  }

  return result;
}
