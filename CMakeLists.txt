cmake_minimum_required(VERSION 3.17)

set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -O3")

project(gstuff)

add_executable(
    ${PROJECT_NAME} gstuff.cpp
)

target_link_libraries(${PROJECT_NAME} 
    X11
)
