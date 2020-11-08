
add_executable( unidiff
                common.cpp
                unidiff.cpp
              )

if ( MSVC )

  target_compile_definitions( unidiff PRIVATE
                              _CRT_SECURE_NO_WARNINGS
                              DTL_MAIN
                            )

else ( )

    target_compile_definitions( unidiff PRIVATE
                                DTL_MAIN
                              )

endif( )

target_include_directories( unidiff PRIVATE
                            ..
                          )

add_library( unidifflib
             unidiff.cpp
           )

target_include_directories( unidifflib PRIVATE
                            ..
                          )

if ( MSVC )

  target_compile_definitions( unidifflib PRIVATE
                               _CRT_SECURE_NO_WARNINGS
                             )

endif( )
