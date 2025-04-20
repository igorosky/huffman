cmake_minimum_required(VERSION 3.0)
project(HuffmanEncoder CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -pedantic -Werror -Wconversion -Wsign-conversion)

add_library(
    HuffmanEncoder
    INTERFACE
)

target_include_directories(HuffmanEncoder INTERFACE include)

if(CMAKE_TESTING_ENABLED)
    enable_testing()
    find_package(GTest REQUIRED)
    include_directories(${GTEST_INCLUDE_DIRS})
    file(GLOB_RECURSE SRC_FILES test/*.cpp)
    foreach(SRC_FILE ${SRC_FILES})
        get_filename_component(TEST_NAME ${SRC_FILE} NAME_WE)
        add_executable(${TEST_NAME} ${SRC_FILE})
        target_link_libraries(${TEST_NAME} HuffmanEncoder ${GTEST_BOTH_LIBRARIES})
        add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    endforeach()
endif()
