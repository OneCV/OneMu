cmake_minimum_required(VERSION 2.8)
PROJECT(testModule)
file(GLOB testModule_SRC
	"*.h"
	"*.c"
)

INCLUDE_DIRECTORIES(
	../../mucore/include
	../../mugadget/include
)


if(WIN32)
SET(OneMu_LIBS ../../../build/out/Debug)
ADD_LIBRARY(oneMuLib STATIC IMPORTED)
SET_PROPERTY(TARGET oneMuLib PROPERTY IMPORTED_LOCATION ${OneMu_LIBS}/OneMu.lib)
add_executable(testModule ${testModule_SRC})
TARGET_LINK_LIBRARIES(testModule oneMuLib)
endif(WIN32)
if(UNIX)
SET(OneMu_LIBS ../../../build/out)
ADD_LIBRARY(oneMuLib STATIC IMPORTED)
SET_PROPERTY(TARGET oneMuLib PROPERTY IMPORTED_LOCATION ${OneMu_LIBS}/libOneMu.a)
add_executable(testModule ${testModule_SRC})
TARGET_LINK_LIBRARIES(testModule oneMuLib m)
endif(UNIX)
