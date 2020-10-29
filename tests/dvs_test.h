/*
 * dvs_test.h:  public interfaces for test programs
 *
 */

#pragma once

#include <cstdint>

typedef char * dvs_error_t;

dvs_error_t dvs_error_create( const char * file, int line, const char *errmsg );

#define DVS_NO_ERROR ((dvs_error_t) 0 )

#define DVS_ERROR(errstr) return( dvs_error_create(__FILE__, __LINE__, errstr ) )

#define DVS_WARN(errstr) { printf( "WARN:  %s(%d): %s\n", __FILE__, __LINE__, errstr ); }

/* Prototype for test driver functions. */
typedef dvs_error_t (*dvs_test_driver_t) ( );

/* All DVS test programs include an array of test descriptors
   (all of our sub-tests) that begins and ends with a DVS_TEST_NULL entry. */
struct dvs_test_descriptor_t
{
  dvs_test_driver_t func;       /* A pointer to the test driver function. */
  int xfail;                    /* Is the test marked XFAIL? */
  const char *msg;              /* Human readable name of test */
};

extern struct dvs_test_descriptor_t test_funcs[];

/* A null initializer for the test descriptor. */
#define DVS_TEST_NULL { NULL, 0, "" }

/* Initializers for PASS and XFAIL tests */
#define DVS_TEST_PASS(  func, msg ) { func, 0, msg }
#define DVS_TEST_XFAIL( func, msg ) { func, 1, msg }

/* Return a pseudo-random number based on SEED, and modify SEED.  This
   is a "good" pseudo-random number generator, intended to replace all
   those "bad" rand() implementations out there. */
uint32_t dvs_test_rand ( uint32_t *seed );

bool ChangeToTestDataDir( char *testDataDirPtr );

bool EqualWithinTolerance( double val1, double val2, double tol );
