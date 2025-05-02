cmake_minimum_required( VERSION 3.28 )

if ( APPLE )

  message( STATUS "Set up for Apple (universal) platform build." )

  set( CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "" FORCE )

  #set( CMAKE_C_COMPILER /usr/local/opt/llvm/bin/clang )
  #set( CMAKE_CXX_COMPILER /usr/local/opt/llvm/bin/clang++ )

endif( )

project( dvs )

option( USE_BOOST_REGEX "Replace std::regex with Boost.Regex" OFF )

# Set standard C++ 23
set( CMAKE_CXX_STANDARD          23  )
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
