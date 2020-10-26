
set( SOURCE_FILES
     command_cat.cpp
     command_hash.cpp
     command_init.cpp
     dvs.cpp
     main.cpp
   )

set( HEADER_FILES
     command_cat.h
     command_hash.h
     command_init.h
     dvs.h
   )

add_executable( dvs
                ${SOURCE_FILES}
                ${HEADER_FILES}
              )

target_include_directories( dvs PRIVATE
                            ../lib/docopt
                            ../lib/SHA-3/SHA-3
                          )

target_link_libraries( dvs
                       docopt_s
                       sha3lib
                     )