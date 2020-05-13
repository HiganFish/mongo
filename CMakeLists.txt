cmake_minimum_required(VERSION 2.6)
project(mongo C CXX)

set(CXX_FLAGS
        -g
        -D_FILE_OFFSET_BITS=64
        -Wall # show all errror
        -Werror # Make all warnings into errors.
        -Wextra # This enables some extra warning flags that are not enabled by -Wall
        -Wconversion # Warn for implicit conversions that may alter a value
        -Wunused-parameter
        -std=c++11
)

find_package(Protobuf)
find_package(ZLIB)

include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(mongo/base)
add_subdirectory(mongo/net)
add_subdirectory(mongo/net/http)
add_subdirectory(examples)

if(PROTOBUF_FOUND)
    add_subdirectory(mongo/net/protobuf)
endif()