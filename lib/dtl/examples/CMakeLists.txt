
add_executable( unidiff
                common.cpp
                unidiff.cpp
              )

if ( MSVC )

  target_compile_definitions( unidiff PRIVATE
                              _CRT_SECURE_NO_WARNINGS
                            )

endif( )

target_include_directories( unidiff PRIVATE
                            ..
                          )
