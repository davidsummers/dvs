project( sha3 )

set( SOURCE_FILES
     Base64.cpp
     CommandParser.cpp
     HashFunction.cpp
     Keccak.cpp
     stdafx.cpp
   )

set( INCLUDE_FILES
     Base64.h
     CommandParser.h
     Endian.h
     HashFunction.h
     Hex.h
     Keccak.h
     ParserCommon.h
     RAII_Wrapper.h
     resource.h
     Rotation.h
     stdafx.h
     targetver.h
)

add_library( sha3lib
             ${SOURCE_FILES}
             ${INCLUDE_FILES}
           )

if ( MSVC )

  target_compile_definitions( sha3lib PRIVATE
                              _CRT_SECURE_NO_WARNINGS
                            )

endif( )

add_executable( sha3-test
                HashingTests.cpp
                Main.cpp
              )

target_link_libraries( sha3-test PRIVATE
                       sha3lib
                     )

if ( MSVC )

  target_compile_definitions( sha3-test PRIVATE
                              _CRT_SECURE_NO_WARNINGS
                            )

endif( )
