#include <filesystem>
#include <iostream>
#include <sstream>

#include "command_init.h"
#include "dvs.h"


std::string InitCommand::operator ( ) ( DVS &dvs_ )
{
  std::cout << "Initializing..." << std::endl;

  std::filesystem::path rootPath = DVS_DIR;

  // Create initial control directory structure
  if ( std::filesystem::exists( rootPath ) &&
       std::filesystem::is_directory( rootPath ) )
  {
    std::stringstream ss;
    ss << "Directory " << DVS_DIR << " already exists.";
    return ss.str( );
  }

  // Create DVS directory.
  if ( !std::filesystem::create_directory( rootPath ) )
  {
    std::stringstream ss;
    ss << "Can't create directory '" << rootPath << "'";
    return ss.str( );
  }

  // Create DVS/objects directory.
  std::filesystem::path objectsDir = rootPath;
  objectsDir /= "objects";

  if ( !std::filesystem::create_directory( objectsDir ) )
  {
    std::stringstream ss;
    ss << "Can't create directory '" << objectsDir << "'";
    return ss.str( );
  }


  if ( std::string validate_error = dvs_.Validate( );
       !validate_error.empty( ) )
  {
    return validate_error;
  }

  std::cout << "Initialized empty DVS repository in " << std::filesystem::absolute( rootPath ) << std::endl;

  return ""; // No error.
}
