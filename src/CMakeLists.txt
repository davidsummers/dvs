
set( SOURCE_FILES
     command_branch_create.cpp
     command_cat.cpp
     command_checkout.cpp
     command_commit.cpp
     command_hash.cpp
     command_init.cpp
     command_log.cpp
     command_read_tree.cpp
     command_status.cpp
     command_tag.cpp
     command_write_tree.cpp
     dvs.cpp
     file_system_base.cpp
     file_system_memory.cpp
     file_system_normal.cpp
     read_tree_generator.cpp
     record_commit.cpp
     record_tree.cpp
   )

set( HEADER_FILES
     command_branch_create.h
     command_cat.h
     command_checkout.h
     command_commit.h
     command_hash.h
     command_init.h
     command_log.h
     command_read_tree.h
     command_status.h
     command_tag.h
     command_write_tree.h
     common.h
     dvs.h
     file_system_base.h
     file_system_memory.h
     file_system_normal.h
     read_tree_generator.h
     record_commit.h
     record_tree.h
   )


# DVS LIBRARY

add_library( dvslib
             ${SOURCE_FILES}
             ${HEADER_FILES}
           )

target_compile_definitions( dvslib PRIVATE
                           _CRT_SECURE_NO_WARNINGS
                         )


target_include_directories( dvslib PRIVATE
                            ../lib/docopt
                            ../lib/SHA-3/SHA-3
                          )

# DVS command

add_executable( dvs
                main.cpp
              )

target_include_directories( dvs PRIVATE
                            ../lib/docopt
                            ../lib/SHA-3/SHA-3
                          )

target_link_libraries( dvs
                       dvslib
                       docopt_s
                       sha3lib
                     )
