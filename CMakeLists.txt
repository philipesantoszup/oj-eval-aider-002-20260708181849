cmake_minimum_required(VERSION 3.10)
project(int2048)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src/include)

add_library(int2048 src/int2048.cpp)

# Optional: Add a test executable if needed
# add_executable(test_int2048 tests/test.cpp)
# target_link_libraries(test_int2048 int2048)
