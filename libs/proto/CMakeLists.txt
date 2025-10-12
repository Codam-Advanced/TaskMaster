cmake_minimum_required(VERSION 3.22)

# Define the library name
set(LIBRARY_NAME proto)

# Set project info
project(${LIBRARY_NAME} VERSION 1.0.0 DESCRIPTION "Protocol Buffers lib for TaskMaster")

set(CMAKE_CXX_STANDARD 23)

# Define the lib directory
set(LIB_DIR .)

find_package(Protobuf REQUIRED)

# Specify the .proto files
set(PROTO_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/taskmaster.proto
)

# Define output directory for generated files
set(PROTO_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/proto)

# Create the output directory if it doesn't exist
file(MAKE_DIRECTORY ${PROTO_OUT_DIR})

# Generate C++ source files from .proto files
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES}
    PROTOC_OUT_DIR ${PROTO_OUT_DIR}
)

# Create the static library
add_library(${LIBRARY_NAME} STATIC ${PROTO_SRCS} ${PROTO_HDRS})

# Specify the include header directory
target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${PROTOBUF_INCLUDE_DIRS})

# Link the Protobuf libraries and Abseil dependencies
target_link_libraries(${LIBRARY_NAME} PUBLIC 
    ${Protobuf_LIBRARIES}
    absl_log_internal_check_op
    absl_log_internal_message
    absl_log_internal_nullguard
)

