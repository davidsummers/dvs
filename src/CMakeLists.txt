
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
