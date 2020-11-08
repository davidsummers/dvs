
add_executable( unidiff
                common.cpp
                unidiff.cpp
              )

target_include_directories( unidiff PRIVATE
                            ..
                          )
