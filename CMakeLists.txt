project(task2)
cmake_minimum_required(VERSION 2.8)

# OpenCV
FIND_PACKAGE(OpenCV REQUIRED)
IF (OpenCV_FOUND)
  MESSAGE(STATUS "OpenCV was found.")
  INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIR})
ELSE (OpenCV_FOUND)
  MESSAGE(FATAL_ERROR "OpenCV was not found.")
ENDIF (OpenCV_FOUND)

SET(CMAKE_SOURCE_DIR source)

FILE( GLOB TASK2_SRC     source/*.cpp )
FILE( GLOB TASK2_HEADERS source/*.h )
FILE( GLOB TCLAP_HEADERS source/tclap/*.h )


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g3 -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${OpenMP_CXX_FLAGS} -O3 -Wall")

if( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE Debug )
endif()

IF(CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]")
  add_definitions(-D_DEBUG)
endif()

include_directories( ${CMAKE_SOURCE_DIR})
link_directories( ${CMAKE_BINARY_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
add_executable( ${PROJECT_NAME} ${TASK2_HEADERS} ${TASK2_SRC} ${TCLAP_HEADERS})

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})


