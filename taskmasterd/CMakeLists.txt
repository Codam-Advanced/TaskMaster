# cmake_minimum_required
cmake_minimum_required(VERSION 3.22)

#define the executable
add_executable(taskmasterd)

# gather all cpp files in src
# GLOB_RECURSE will search recursively in all subdirectories
file(GLOB_RECURSE SOURCES "src/*.cpp")

#add executable sources
target_sources(taskmasterd PRIVATE ${SOURCES})

# include header directory
target_include_directories(taskmasterd PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

#include FetchContent module
include(FetchContent)

# Fetch yaml-cpp library
FetchContent_Declare(
    yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG        0.8.0
)
FetchContent_MakeAvailable(yaml-cpp)

# link yaml-cpp library
target_link_libraries(taskmasterd PUBLIC yaml-cpp)

# Set C++ standard to C++20
set_target_properties(taskmasterd PROPERTIES CXX_STANDARD 20)



# the PRIVATE keyword means that the target source and include directories will
# only be used for this target and not for any targets that link to it.