cmake_minimum_required( VERSION 2.8.12 )

project( dvs )

option( USE_BOOST_REGEX "Replace std::regex with Boost.Regex" OFF )

# Set standard C++ 17
set( CMAKE_CXX_STANDARD          17  )
set( CMAKE_CXX_STANDARD_REQUIRED ON  )
set( CMAKE_CXX_EXTENSIONS        OFF )

if ( UNIX AND NOT APPLE )
  message( "**** GCC COMPILER VERSION: ${CMAKE_CXX_COMPILER_VERSION}" )
endif( )

add_subdirectory( lib/docopt )
add_subdirectory( lib/dtl )
add_subdirectory( lib/SHA-3/SHA-3 )
add_subdirectory( src )
add_subdirectory( tests )
