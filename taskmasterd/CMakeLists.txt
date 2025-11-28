# set the executable name
set(EXECUTABLE_NAME taskmasterd)

# define the executable
add_executable(${EXECUTABLE_NAME})

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

# gather all cpp files in src
# GLOB_RECURSE will search recursively in all subdirectories
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Add compile options
set(COMPILE_OPTIONS -Wall -Wextra -Werror -Wno-gnu-zero-variadic-macro-arguments -Wno-gcc-compat -pedantic -O3 -march=native -g -DPROGRAM_NAME="${EXECUTABLE_NAME}")
target_compile_options(${EXECUTABLE_NAME} PRIVATE ${COMPILE_OPTIONS})

# target_link_options(${EXECUTABLE_NAME} PRIVATE -fsanitize=address)

# Link to the needed libs
target_link_libraries(${EXECUTABLE_NAME} PRIVATE logger utils ipc)

# add executable sources
target_sources(${EXECUTABLE_NAME} PRIVATE ${SOURCES})

# include header directory
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)

# the PRIVATE keyword means that the target source and include directories will
# only be used for this target and not for any targets that link to it.
