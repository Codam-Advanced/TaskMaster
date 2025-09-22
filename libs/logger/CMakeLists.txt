cmake_minimum_required(VERSION 3.22)

# Define the library name
set(LIBRARY_NAME logger)

# Set project info
project(${LIBRARY_NAME} VERSION 1.0.0 DESCRIPTION "Logging lib for TaskMaster")

set(CMAKE_CXX_STANDARD 23)

# Define the lib directory
set(LIB_DIR .)

# Collect all source files
file(GLOB_RECURSE LIB_SOURCES "${LIB_DIR}/*.cpp")

# Create the static library
ADD_LIBRARY(${LIBRARY_NAME} STATIC ${LIB_SOURCES})

# Specify the internal include dir
target_include_directories(${LIBRARY_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Specify the include header directory
target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
