
PROJECT(OneMu)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

#include path
INCLUDE_DIRECTORIES(
	${PROJECT_SOURCE_DIR}/mucore/include
	${PROJECT_SOURCE_DIR}/mugadget/include
)

SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/out)
SET(INCLUDE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/include)

ADD_SUBDIRECTORY(mucore)
ADD_SUBDIRECTORY(mugadget)

SET(INCLUDE_ALL_FILES
${PROJECT_SOURCE_DIR}/mucore/include/muBase.h
${PROJECT_SOURCE_DIR}/mucore/include/muCore.h
${PROJECT_SOURCE_DIR}/mucore/include/muTypes.h
${PROJECT_SOURCE_DIR}/mugadget/include/muGadget.h
)

INSTALL(FILES ${INCLUDE_ALL_FILES} DESTINATION ${INCLUDE_OUTPUT_PATH})

