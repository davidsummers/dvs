#include <filesystem>
#include <iostream>
#include <sstream>
#include <sys/stat.h>

#include "dvs.h"

DVS::DVS( )
{
}


DVS::~DVS( )
{
}


std::string DVS::Init( )
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

  std::string validate_error;
  validate_error = Validate( );

  if ( !validate_error.empty( ) )
  {
    std::stringstream ss;
    ss << "Can't validate " << DVS_DIR << " directory: " + validate_error;
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

  std::cout << "Initialized empty DVS repository in " << std::filesystem::absolute( rootPath ) << std::endl;

  return ""; // No error.
}


std::string DVS::Status( )
{
  std::cout << "Status: " << std::endl;
  return ""; // No error.
}


std::string DVS::Validate( )
{
  return ""; // No errors.
}
