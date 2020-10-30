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
#include "command_cat.h"
#include "command_init.h"
#include "command_hash.h"
#include "command_read_tree.h"
#include "command_write_tree.h"


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

    const char *EXPECTED_FILE_NAME = "README.txt";
    const char *EXPECTED_TEXT = "This is a test file.\n";
    {
      std::ofstream readmeFile( EXPECTED_FILE_NAME, std::ios_base::binary );
      readmeFile << EXPECTED_TEXT;
    }

    HashCommand hashCommand;

    OidResult result = hashCommand.Hash( dvs, EXPECTED_FILE_NAME );
    
    if ( !result.err.empty( ) )
    {
      DVS_ERROR( result.err.c_str( ) );
    }

    const char *EXPECTED_OID = "330f2ed27c77204cac1ab567cfffbad0db5724f8254c2e9de155da5979c401d6";
    if ( result.oid != EXPECTED_OID )
    {
      std::stringstream ss;
      ss << "Expected: " << EXPECTED_OID << ", but got " << result.oid << std::endl;
      DVS_ERROR( ss.str( ).c_str( ) );
    }
  }

  return DVS_NO_ERROR;
}


// Tests the 'dvs internal cat <hash>' command.
static dvs_error_t test_dvs_internal_cat( )
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

    const char *EXPECTED_FILE_NAME = "README.txt";
    const char *EXPECTED_TEXT = "This is a test file.\r\n";
    {
      std::ofstream readmeFile( EXPECTED_FILE_NAME, std::ios_base::binary );
      readmeFile << EXPECTED_TEXT;
    }

    HashCommand hashCommand;

    OidResult result = hashCommand.Hash( dvs, EXPECTED_FILE_NAME );
    
    if ( !result.err.empty( ) )
    {
      DVS_ERROR( result.err.c_str( ) );
    }

    const char *EXPECTED_OID = "4489764f4231744fc5d7310cb208657282cfe45d943a65ac69b4d6404763193c";
    if ( result.oid != EXPECTED_OID )
    {
      std::stringstream ss;
      ss << "Expected: " << EXPECTED_OID << ", but got " << result.oid << std::endl;
      DVS_ERROR( ss.str( ).c_str( ) );
    }

    CatCommand catCommand;

    std::stringstream outputStream;

    CatCommand::CatResult catResult = catCommand.GetHash( dvs, EXPECTED_OID, &outputStream );
 
    const std::string expectedType = "blob";
    const size_t expectedSize = 22;

    if ( catResult.type != expectedType )
    {
      std::stringstream ss;
      ss << "Expected type '" << expectedType << "' but got '" << catResult.type << "'." << std::endl;
      DVS_ERROR( ss.str( ).c_str( ) );
    }

    if ( catResult.size != expectedSize )
    {
      std::stringstream ss;
      ss << "Expected size " << expectedSize << " but got " << catResult.size << "." << std::endl;
      DVS_ERROR( ss.str( ).c_str( ) );
    }

    if ( expectedSize != outputStream.str( ).size( ) )
    {
      std::stringstream ss;
      ss << "Expected stream size " << expectedSize << " but got " << outputStream.str( ).size( ) << "." << std::endl;
      DVS_ERROR( ss.str( ).c_str( ) );
    }

    if ( outputStream.str( ) != EXPECTED_TEXT )
    {
      std::stringstream ss;
      ss << "Expected: '" << EXPECTED_TEXT << "' but got '" << outputStream.str( ) << "'." << std::endl;
      DVS_ERROR( ss.str( ).c_str( ) );
    }
  }

  return DVS_NO_ERROR;
}


// Tests the 'dvs internal write-tree' command.
static dvs_error_t test_dvs_internal_write_tree( )
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

    const char *EXPECTED_FILE_NAME = "README.txt";
    const char *EXPECTED_TEXT = "This is a test file.\r\n";
    {
      std::ofstream readmeFile( EXPECTED_FILE_NAME, std::ios_base::binary );
      readmeFile << EXPECTED_TEXT;
    }

    std::filesystem::create_directory( "dir1" );

    const std::string EXPECTED_FILE_NAME_2 = "FILE_2.txt";
    const char *EXPECTED_TEXT_2 = "This is another test file.\r\n";
    {
      std::ofstream readmeFile2( "dir1/" + EXPECTED_FILE_NAME_2, std::ios_base::binary );
      readmeFile2 << EXPECTED_TEXT_2;
    }

    WriteTreeCommand writeTreeCommand;

    const std::string expectedDirOid = "ef9f7e727e6cf95bb64f8dc2b46e398fff320ea6eeed9601679db3553feab54c";
    OidResult result = writeTreeCommand.WriteTree( dvs, "." );
    
    if ( !result.err.empty( ) )
    {
      DVS_ERROR( result.err.c_str( ) );
    }

    if ( result.oid != expectedDirOid )
    {
      std::stringstream ss;
      ss << "Expected dir oid '" << expectedDirOid << "' but got '" << result.oid << "'" << std::endl;
      DVS_ERROR( ss.str( ).c_str( ) );
    }
  }

  return DVS_NO_ERROR;
}


// Tests the 'dvs internal read-tree' command.
static dvs_error_t test_dvs_internal_read_tree( )
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

    const char *EXPECTED_FILE_NAME = "README.txt";
    const char *EXPECTED_TEXT = "This is a test file.\r\n";
    {
      std::ofstream readmeFile( EXPECTED_FILE_NAME, std::ios_base::binary );
      readmeFile << EXPECTED_TEXT;
    }

    std::filesystem::create_directory( "dir1" );

    const std::string EXPECTED_FILE_NAME_2 = "FILE_2.txt";
    const char *EXPECTED_TEXT_2 = "This is another test file.\r\n";
    {
      std::ofstream readmeFile2( "dir1/" + EXPECTED_FILE_NAME_2, std::ios_base::binary );
      readmeFile2 << EXPECTED_TEXT_2;
    }

    WriteTreeCommand writeTreeCommand;

    const std::string expectedDirOid = "ef9f7e727e6cf95bb64f8dc2b46e398fff320ea6eeed9601679db3553feab54c";
    OidResult result = writeTreeCommand.WriteTree( dvs, "." );
    
    if ( !result.err.empty( ) )
    {
      DVS_ERROR( result.err.c_str( ) );
    }

    if ( result.oid != expectedDirOid )
    {
      std::stringstream ss;
      ss << "Expected dir oid '" << expectedDirOid << "' but got '" << result.oid << "'" << std::endl;
      DVS_ERROR( ss.str( ).c_str( ) );
    }

    ReadTreeCommand readTreeCommand;
    OidResult readResult = readTreeCommand.ReadTree( dvs, expectedDirOid );

    if ( !readResult.err.empty( ) )
    {
      DVS_ERROR( readResult.err.c_str( ) );
    }
  }

  return DVS_NO_ERROR;
}

/* The test table.  */

struct dvs_test_descriptor_t test_funcs[] =
{
  DVS_TEST_NULL,
  DVS_TEST_PASS( test_dvs_init,                "Test DVS init command."                ),
  DVS_TEST_PASS( test_dvs_internal_hash,       "Test DVS internal hash command."       ),
  DVS_TEST_PASS( test_dvs_internal_cat,        "Test DVS internal cat <hash> command." ),
  DVS_TEST_PASS( test_dvs_internal_write_tree, "Test DVS internal write-tree command." ),
  DVS_TEST_PASS( test_dvs_internal_read_tree,  "Test DVS internal read-tree command."  ),
  DVS_TEST_NULL
};
