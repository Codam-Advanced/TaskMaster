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

# Set C++ standard to C++20
set_target_properties(taskmasterd PRIVATE cxx_std_20)

# the PRIVATE keyword means that the target source and include directories will
# only be used for this target and not for any targets that link to it.