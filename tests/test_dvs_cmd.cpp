//
// test_date.cpp --- test FavorDate class.
//

#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#endif

#include <fstream>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sstream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <iostream>
#ifndef WIN32
#include <unistd.h>
#endif

#include "dvs.h"
#include "dvs_test.h"
#include "command_init.h"

// The tests.

// Tests the 'dvs init' command.
static dvs_error_t test_dvs_init( )
{
  std::filesystem::remove_all( "test" );
  {
    DVS dvs;

    InitCommand initCommand;

    std::string err = initCommand.InitDvs( dvs, "test" );

    if ( !err.empty( ) )
    {
      std::stringstream ss;
      ss << "Error creating dvs 'test' directory: " << err << std::endl; 
      DVS_ERROR( ss.str( ).c_str( ) );
    }
  }

  return DVS_NO_ERROR;
}


// Tests the 'dvs internal hash README.txt' command.
static dvs_error_t test_dvs_internal_hash( )
{
  std::filesystem::remove_all( "test" );
  {
    DVS dvs;

    InitCommand initCommand;

    std::string err = initCommand.InitDvs( dvs, "test" );

    if ( !err.empty( ) )
    {
      std::stringstream ss;
      ss << "Error creating dvs 'test' directory: " << err << std::endl; 
      DVS_ERROR( ss.str( ).c_str( ) );
    }

    std::filesystem::current_path( "test" );

    std::ofstream readmeFile( "README.txt" );
    readmeFile << "This is a README.txt file." << std::endl;
  }

  return DVS_NO_ERROR;
}


/* The test table.  */

struct dvs_test_descriptor_t test_funcs[] =
{
  DVS_TEST_NULL,
  DVS_TEST_PASS( test_dvs_init, "Test DVS init command." ),
  DVS_TEST_PASS( test_dvs_init, "Test DVS internal hash command." ),
  DVS_TEST_NULL
};