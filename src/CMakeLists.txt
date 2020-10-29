
set( SOURCE_FILES
     command_cat.cpp
     command_hash.cpp
     command_init.cpp
     command_read_tree.cpp
     command_status.cpp
     command_write_tree.cpp
     dvs.cpp
     file_system_base.cpp
     file_system_memory.cpp
     file_system_normal.cpp
     main.cpp
     read_tree_generator.cpp
   )

set( HEADER_FILES
     command_cat.h
     command_hash.h
     command_init.h
     command_read_tree.h
     command_status.h
     command_write_tree.h
     common.h
     dvs.h
     file_system_base.h
     file_system_memory.h
     file_system_normal.h
     read_tree_generator.h
   )

add_executable( dvs
                ${SOURCE_FILES}
                ${HEADER_FILES}
              )

target_compile_definitions( dvs PRIVATE
                           _CRT_SECURE_NO_WARNINGS
                         )

target_include_directories( dvs PRIVATE
                            ../lib/docopt
                            ../lib/SHA-3/SHA-3
                          )

target_link_libraries( dvs
                       docopt_s
                       sha3lib
                     )
