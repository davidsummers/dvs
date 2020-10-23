
set( SOURCE_FILES
     dvc.cpp
     main.cpp
   )

set( HEADER_FILES
     dvc.h
   )

add_executable( dvs
                ${SOURCE_FILES}
                ${HEADER_FILES}
              )
