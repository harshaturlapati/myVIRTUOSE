cmake_minimum_required(VERSION 2.6)

# Set package properties
project(myVirtuose)

set(CMAKE_INCLUDE_CURRENT_DIR   ON)
set(CMAKE_VERBOSE_MAKEFILE      ON)
set(CMAKE_COLOR_MAKEFILE        ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Activate C++ 11
set (CMAKE_CXX_STANDARD 11)

# Locally-developed modules dist'ed with this app - always have this first.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/libs")

# Create executable for each example
# Look for examples under folders
file(GLOB EXE_LIST RELATIVE ${PROJECT_SOURCE_DIR} "[0-9]*-*/[0-9]*.cpp")
foreach ( SRC_FILE ${EXE_LIST} )

  string(REPLACE ".cpp" "" TARGET_EXE_NAME ${SRC_FILE})
  string(REPLACE "/" "_" TARGET_EXE_NAME ${TARGET_EXE_NAME})
  
  MESSAGE("creating TARGET_EXE_NAME: '${TARGET_EXE_NAME}'")

  add_executable(${TARGET_EXE_NAME} ${SRC_FILE})
  target_link_libraries(${TARGET_EXE_NAME} "${PROJECT_SOURCE_DIR}/libs/virtuoseDLL.lib")

endforeach()