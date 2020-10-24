
set( SOURCE_FILES
     dvs.cpp
     main.cpp
   )

set( HEADER_FILES
     dvs.h
   )

add_executable( dvs
                ${SOURCE_FILES}
                ${HEADER_FILES}
              )

target_include_directories( dvs PRIVATE
                            ../lib/docopt
                          )

target_link_libraries( dvs
                       docopt_s
                     )