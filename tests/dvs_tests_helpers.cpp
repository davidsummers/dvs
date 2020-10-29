#ifdef WIN32

#include <winsock2.h>
#include <windows.h>

#include <direct.h>
#include <iostream>
#include <string>

#include "dvs_test.h"


//////////////////////////////////////////////////////////////////////
//
//  ChangeToTestDataDir( )
//
//  Description: Find path to test data.
//
//////////////////////////////////////////////////////////////////////
bool ChangeToTestDataDir( char *testDataDirPtr_ )
{
  std::string findstr = "\\source";
  std::string currentDirectory = _getcwd( 0, 2048 );
  size_t pos;

  // std::cout << "Current Directory: " << currentDirectory << std::endl;

  int found = 0;

  pos = currentDirectory.rfind( findstr );

  if ( pos <= 0 )
  {
    std::cout << "Can't find source directory." << std::endl;
    return( false );
  }

  currentDirectory = currentDirectory.substr( 0, pos + findstr.size() );
  currentDirectory += testDataDirPtr_; // Bump to previous directory for config files.

  if ( _chdir( currentDirectory.c_str() ) != 0 )
  {
    std::cout << "Can't change to " << currentDirectory << std::endl;
    return( false ); // Error.
  }

  return( true ); // No error.
}
#endif // WIN32
