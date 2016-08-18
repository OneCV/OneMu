
PROJECT(OneMu)

SET(OneMu_SRCS
 src/muBase.c
 src/muColortransform.c
 src/muComponent.c
 src/muEdge.c
 src/muFilter.c
 src/muHistogram.c
 src/muImgwarp.c
 src/muLogic.c
 src/muMorphological.c
 src/muMotion.c
 src/muThreshold.c
 src/muMatching.c
)

if (WIN32 OR UNIX)
ADD_DEFINITIONS(-DGENERIC)
endif (WIN32 OR UNIX)


ADD_LIBRARY(OneMuStatic STATIC ${OneMu_SRCS})
ADD_LIBRARY(OneMu SHARED ${OneMu_SRCS})
set_target_properties(OneMuStatic PROPERTIES OUTPUT_NAME OneMu)
add_dependencies(OneMu OneMuStatic)
