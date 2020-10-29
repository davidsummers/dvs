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
#include "command_hash.h"


const char *TEST_DIR = "test";

// The tests.

// Tests the 'dvs init' command.
static dvs_error_t test_dvs_init( )
{
  std::filesystem::remove_all( TEST_DIR );
  {
    DVS dvs;

    InitCommand initCommand;

    std::string err = initCommand.InitDvs( dvs, TEST_DIR );

    if ( !err.empty( ) )
    {
      std::stringstream ss;
      ss << "Error creating dvs '" << TEST_DIR << "' directory: " << err << std::endl; 
      DVS_ERROR( ss.str( ).c_str( ) );
    }
  }

  return DVS_NO_ERROR;
}


// Tests the 'dvs internal hash README.txt' command.
static dvs_error_t test_dvs_internal_hash( )
{
  std::filesystem::remove_all( TEST_DIR );
  {
    DVS dvs;

    InitCommand initCommand;

    std::string err = initCommand.InitDvs( dvs, TEST_DIR );

    if ( !err.empty( ) )
    {
      std::stringstream ss;
      ss << "Error creating dvs '" << TEST_DIR << "' directory: " << err << std::endl; 
      DVS_ERROR( ss.str( ).c_str( ) );
    }

    std::filesystem::current_path( TEST_DIR );

    const char *README = "README.txt";

    {
      std::ofstream readmeFile( README );
      readmeFile << "This is a " << README << " file." << std::endl;
    }

    HashCommand hashCommand;

    OidResult result = hashCommand.Hash( dvs, README );
    
    if ( !result.err.empty( ) )
    {
      DVS_ERROR( result.err.c_str( ) );
    }

    const char *EXPECTED_OID = "996d18c4220e5b4386e1ddeba480b2fc6e54e4d986ecf3634b834122c7fafdff";
    if ( result.oid != EXPECTED_OID )
    {
      std::stringstream ss;
      ss << "Expected: " << EXPECTED_OID << ", but got " << result.oid << std::endl;
      DVS_ERROR( ss.str( ).c_str( ) );
    }
  }

  return DVS_NO_ERROR;
}


/* The test table.  */

struct dvs_test_descriptor_t test_funcs[] =
{
  DVS_TEST_NULL,
  DVS_TEST_PASS( test_dvs_init,          "Test DVS init command."          ),
  DVS_TEST_PASS( test_dvs_internal_hash, "Test DVS internal hash command." ),
  DVS_TEST_NULL
};
