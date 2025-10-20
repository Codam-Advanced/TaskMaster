# Define the library name
set(LIBRARY_NAME logger)

# Set project info
project(${LIBRARY_NAME} VERSION 1.0.0 DESCRIPTION "Utility lib for TaskMaster")

# Define the lib directory
set(LIB_DIR .)

# Collect all source files
file(GLOB_RECURSE LIB_SOURCES "${LIB_DIR}/*.cpp")

# Create the static library
add_library(${LIBRARY_NAME} STATIC ${LIB_SOURCES})

# Specify the include header directory
target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
