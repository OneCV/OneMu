
PROJECT(OneMuGadget)

SET(OneMuGadget_SRCS
src/muBackgroundmodeling.c                                              
src/muCameratampering.c
src/muObjectdetector.c
src/muExaminator.c
)

if (WIN32 OR UNIX)
ADD_DEFINITIONS(-DGENERIC)
endif (WIN32 OR UNIX)


ADD_LIBRARY(OneMuGadgetStatic STATIC  ${OneMuGadget_SRCS})
ADD_LIBRARY(OneMuGadget SHARED ${OneMuGadget_SRCS})
set_target_properties(OneMuGadgetStatic PROPERTIES OUTPUT_NAME OneMuGadget)
target_link_libraries(OneMuGadget OneMu)
add_dependencies(OneMuGadget OneMu)
