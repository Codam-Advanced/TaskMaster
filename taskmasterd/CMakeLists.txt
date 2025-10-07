# cmake_minimum_required
cmake_minimum_required(VERSION 3.22)

set(EXECUTABLE_NAME taskmasterd)

# define the executable
add_executable(${EXECUTABLE_NAME})

# gather all cpp files in src
# GLOB_RECURSE will search recursively in all subdirectories
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Add compile options
set(COMPILE_OPTIONS -Wall -Wextra -Werror -pedantic -O3 -march=native -g -DPROGRAM_NAME="${EXECUTABLE_NAME}")
target_compile_options(${EXECUTABLE_NAME} PRIVATE ${COMPILE_OPTIONS})

# Enable fsanitize:
# target_link_options(${EXECUTABLE_NAME} PRIVATE -fsanitize=address)

# Link to the needed libs
target_link_libraries(${EXECUTABLE_NAME} PRIVATE logger)

# add executable sources
target_sources(${EXECUTABLE_NAME} PRIVATE ${SOURCES})

# include header directory
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)

# Set C++ standard to C++23
set(CMAKE_CXX_STANDARD 23)

# the PRIVATE keyword means that the target source and include directories will
# only be used for this target and not for any targets that link to it.
