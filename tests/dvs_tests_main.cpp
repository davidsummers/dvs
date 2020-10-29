/*
 * tests-main.c:  shared main() & friends for DVS test-suite programs
 *
 */

#ifdef WIN32

#include <winsock2.h>
#include <windows.h>

#else

#include <unistd.h>

#define Sleep( x ) sleep( x )

#endif

#include <ctype.h>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dvs_test.h"

/* Some DVS test programs may want to parse options in the
   argument list, so we remember it here. */
int test_argc;
char **test_argv;


/* ================================================================= */
/* Quite a few tests use random numbers. */

uint32_t dvs_test_rand ( uint32_t *seed )
{
  *seed = (*seed * 1103515245UL + 12345UL) & 0xffffffffUL;
  return *seed;
}


dvs_error_t dvs_error_create( const char *file, int line, const char *errmsg )
{
  static char buf[BUFSIZ];
  
  sprintf( buf, "%s(%d): %s", file, line, errmsg );

  return &buf[0];
}

bool EqualWithinTolerance( double val1, double val2, double tol )
{
  if ( fabs( val1 - val2 ) <= tol )
  {
    return true;
  }

  return false;
}


/* ================================================================= */


/* Determine the array size of test_funcs[], the inelegant way.  :)  */
#ifndef NO_MAIN
static int get_array_size (void)
{
  int i;

  for ( i = 1; test_funcs[i].func; i++ )
  {
  }

  return (i - 1);
}



/* Execute a test number TEST_NUM.  Pretty-print test name and dots
   according to our test-suite spec, and return the result code. */
static int do_test_num( const char *progname,
                        int test_num,
                        bool msg_only )
{
  dvs_test_driver_t func;
  int xfail;
  dvs_error_t err;
  int array_size = get_array_size();
  const char *msg = 0;  /* the message this individual test prints out */

  /* Check our array bounds! */
  if ((test_num > array_size) || (test_num <= 0))
  {
    printf ("FAIL: %s: THERE IS NO TEST NUMBER %2d\n", progname, test_num);
    return 1;  /* BAIL, this test number doesn't exist. */
  }
  else
  {
    func  = test_funcs[ test_num ].func;
    xfail = test_funcs[ test_num ].xfail;
    msg   = test_funcs[ test_num ].msg;
  }

  if ( !msg_only )
  {
    // Get name
    printf( "START: %s %d: %s:", progname, test_num, msg ? msg : "(test did not provide name)" );
    fflush( stdout );
  }

  /* Do test */
  err = func( );

  /* If we got an error, print it out.  */
  if ( err )
  {
    printf( "\n%s\n", err );
  }

  if ( msg_only )
  {
    printf (" %2d     %5s  %s\n",
            test_num,
            xfail ? "XFAIL" : "NORMAL",
            msg ? msg : "(test did not provide name)");
  }
  else
  {
    // Now we just do CARRIAGE RETURN (no line feed) and re-print with test completion status.
    printf ("\r%s %s %d: %s\n",
            (err
             ? (xfail ? "XFAIL:" : "FAIL :")
             : (xfail ? "XPASS:" : "PASS :")),
            progname,
            test_num,
            msg ? msg : "(test did not provide name)");
  }

  fflush( stdout );

  /* Fail on unexpected result -- FAIL or XPASS. */
  return (err != DVS_NO_ERROR) != (xfail != 0);
}


/* Standard svn test program */
int main ( int argc, char *argv[ ] )
{
  if ( getenv( "DVS_DEBUG" ) )
  {
    volatile bool done = false;
    while ( !done )
    {
      Sleep( 1000 );
      fprintf( stderr, "DVS_DEBUG detected....sleeping.\n" );
    }
  }

  char *prog_name;
  int test_num;
  int i;
  int got_error = 0;
  int ran_a_test = 0;

  /* How many tests are there? */
  int array_size = get_array_size( );
  
  /* Strip off any leading path components from the program name.  */
  prog_name = strrchr( argv[0], '/' );
  if ( prog_name )
  {
    prog_name++;
  }
  else
  {
    /* Just check if this is that weird platform that uses \ instead
       of / for the path separator. */
    prog_name = strrchr( argv[0], '\\' );
    if ( prog_name )
    {
        prog_name++;
    }
    else
    {
        prog_name = argv[ 0 ];
    }
  }

  /* Remember the command line */
  test_argc = argc;
  test_argv = argv;

  if ( argc >= 2 )  /* notice command-line arguments */
  {
    if ( ! strcmp( argv[1], "list" ) )
    {
      ran_a_test = 1;

      /* run all tests with MSG_ONLY set to TRUE */
      printf( "Test #  Mode   Test Description\n"
                 "------  -----  ----------------\n" );
      for ( i = 1; i <= array_size; i++ )
      {
        if ( do_test_num( prog_name, i, true ) )
        {
          got_error = 1;
        }
      }
    }
    else
    {
      for ( i = 1; i < argc; i++ )
      {
        if ( isdigit( argv[ i ][ 0 ] ) )
        {
          ran_a_test = 1;
          test_num = atoi (argv[ i ] );
          if ( do_test_num( prog_name, test_num, false ) )
          {
            got_error = 1;
          }
        }
        else if ( argv[i][0] != '-' )
        {
          /* (probably) a source directory pathname */
          printf( "notice: ignoring argument %d\n", i );
        }
      }
    }
  }
  if ( ! ran_a_test )
  {
    /* just run all tests */
    for ( i = 1; i <= array_size; i++ )
    {
      if ( do_test_num ( prog_name, i, false ) )
      {
        got_error = 1;
      }
    }
  }

  exit( got_error );
  return got_error;
}

#endif // NO_MAIN
