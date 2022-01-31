cmake_minimum_required(VERSION 3.21)
project(testJoystick)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-O3 -no-pie -ggdb -Wall")

include_directories("/usr/local/include/lcm/")   # lcm includes
include_directories(lcmFiles/cpp)

add_executable(main main.cpp)
target_link_libraries(main lcm)
