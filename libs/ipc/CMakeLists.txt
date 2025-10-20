cmake_minimum_required(VERSION 3.22)

# Define the library name
set(LIBRARY_NAME ipc)

# Set project info
project(${LIBRARY_NAME} VERSION 1.0.0 DESCRIPTION "Protocol Buffers lib for TaskMaster")

set(CMAKE_CXX_STANDARD 17)

# Define the lib directory
set(LIB_DIR .)

# Collect all source files
file(GLOB_RECURSE LIB_SOURCES "${LIB_DIR}/*.cpp")

include(FetchContent)

FetchContent_Declare(
    protobuf
    GIT_REPOSITORY https://github.com/protocolbuffers/protobuf.git
    GIT_TAG        v30.0
)

# Disable tests and install
set(protobuf_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(protobuf_INSTALL OFF CACHE BOOL "" FORCE)

# This builds protobuf from source and makes it available
FetchContent_MakeAvailable(protobuf)

# import the protobuf CMake macros.
include(${protobuf_SOURCE_DIR}/cmake/protobuf-generate.cmake)

# Define output directory for generated files
set(PROTO_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/proto)

# Create the output directory if it doesn't exist
file(MAKE_DIRECTORY ${PROTO_BINARY_DIR})

# Create the static library
add_library(${LIBRARY_NAME} STATIC ${LIB_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/proto/taskmaster.proto)

# Generate C++ sources using protoc
protobuf_generate(
    TARGET "${LIBRARY_NAME}"
    IMPORT_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/proto"
    PROTOC_OUT_DIR "${PROTO_BINARY_DIR}"
)

# Specify the internal include dir
target_include_directories(${LIBRARY_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Specify the include header directory
target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${PROTOBUF_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/..)

# Link the Protobuf libraries and Abseil dependencies
target_link_libraries(${LIBRARY_NAME} PUBLIC protobuf::libprotobuf)