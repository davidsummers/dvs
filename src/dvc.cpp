#include <iostream>
#include <sys/stat.h>
#include <unistd.h>

#include "dvc.h"

DVC::DVC( )
{
}


DVC::~DVC( )
{
}


std::string DVC::Init( )
{
  std::cout << "Initializing..." << std::endl;

  // Create initial control directory structure
  if ( access( DVC_DIR, R_OK | W_OK ) == 0 )
  {
    return "Directory .dvc already exists.";
  }

  std::string validate_error;
  validate_error = Validate( ):

  if ( !validate_error.empty( ) ))
  {
    return "Can't validate " + DVC_DIR + "directory: " + validate_error;
  }

  // Create directory and file structure.
  std::string tmp;
  tmp = DVC_DIR;
  mkdir( tmp.c_str( ), 0777 );
  tmp = DVC_DIR + "/objects";
  mkdir( tmp.c_str( ), 0777 );

  return ""; // No error.
}


std::string DVC::Status( )
{
  std::cout << "Status: " << std::endl;
  return ""; // No error.
}


std::string DVC::Validate( )
{
  return ""; // No errors.
}
