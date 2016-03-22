
PROJECT(OneMuGADGET)

SET(OneMuGADGET_SRCS
src/muBackgroundmodeling.c                                              
src/muCameratampering.c
src/muPirisanalyze.c
src/muPtzgettarget.c
src/muPtztracking.c
src/muSadtracking.c
src/muSadtripwirecounting.c
src/muSadtripzone.c
src/muObjectdetector.c
src/muExaminator.c
)

if (WIN32 OR UNIX)
ADD_DEFINITIONS(-DGENERIC)
endif (WIN32 OR UNIX)


ADD_LIBRARY(OneMuGADGETStatic STATIC  ${OneMuGADGET_SRCS})
ADD_LIBRARY(OneMuGADGET SHARED ${OneMuGADGET_SRCS})
set_target_properties(OneMuGADGETStatic PROPERTIES OUTPUT_NAME OneMuGADGET)
target_link_libraries(OneMuGADGET OneMu)
add_dependencies(OneMuGADGET OneMu)
