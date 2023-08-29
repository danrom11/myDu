cmake_minimum_required(VERSION 3.10)
project(MyDu)

set(CMAKE_CXX_STANDART 17)

add_executable(myDu myDu.cpp)
target_link_libraries(myDu stdc++fs)
