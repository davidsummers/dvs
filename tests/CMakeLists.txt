cmake_minimum_required( VERSION 3.6.3 )
project( dvs_tests )

message( "DVS_TESTS: ${CMAKE_CURRENT_SOURCE_DIR}" )

set( DVS_TEST_SOURCE_FILES
     dvs_tests_helpers.cpp
     dvs_tests_main.cpp
   )

set( DVS_TEST_HEADER_FILES
     dvs_test.h
   )

add_executable( test_dvs_command
                test_dvs_cmd.cpp
                ${DVS_TEST_SOURCE_FILES}
                ${DVS_TEST_HEADER_FILES}
              )

target_include_directories( test_dvs_command PRIVATE
                            ../src
                            ../lib/docopt
                            ../lib/SHA-3
                          )

target_link_libraries( test_dvs_command
                       dvslib
                       docopt_s
                       sha3lib
                     )

