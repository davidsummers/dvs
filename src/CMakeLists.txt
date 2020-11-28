if ( NOT GIT_HASH )
  set( GIT_HASH "dev" )
endif( )

message( "GIT_HASH = ${GIT_HASH}" )

set( SOURCE_FILES
     command_base.cpp
     command_branch_create.cpp
     command_branch_delete.cpp
     command_branch_list.cpp
     command_branch_switch.cpp
     command_cat.cpp
     command_commit.cpp
     command_diff.cpp
     command_hash.cpp
     command_init.cpp
     command_log.cpp
     command_read_tree.cpp
     command_status.cpp
     command_tag_create.cpp
     command_tag_delete.cpp
     command_tag_list.cpp
     command_write_tree.cpp
     command_unimplemented.cpp
     diff.cpp
     dvs.cpp
     record_commit.cpp
     record_tree.cpp
   )

set( HEADER_FILES
     command_base.h
     command_branch_create.h
     command_branch_delete.h
     command_branch_list.h
     command_branch_switch.h
     command_cat.h
     command_commit.h
     command_diff.h
     command_hash.h
     command_init.h
     command_log.h
     command_read_tree.h
     command_status.h
     command_tag_create.h
     command_tag_delete.h
     command_tag_list.h
     command_write_tree.h
     command_unimplemented.h
     common.h
     diff.h
     dvs.h
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
                           CMAKE_GIT_HASH=${GIT_HASH}
                         )


target_include_directories( dvslib PRIVATE
                            ../lib/docopt
                            ../lib/dtl
                            ../lib/dtl/examples
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
                       unidifflib
                     )
