cmake_minimum_required(VERSION 3.18)

project(implicit2obj)

set(CMAKE_CXX_STANDARD 11)

add_library(implicit2obj ${CMAKE_SOURCE_DIR}/src/marching_cube.cpp
                         ${CMAKE_SOURCE_DIR}/src/mesh.cpp)
                                
target_include_directories(implicit2obj PUBLIC ${CMAKE_SOURCE_DIR}/include)


add_executable(example example.cpp)
target_link_libraries(example implicit2obj)