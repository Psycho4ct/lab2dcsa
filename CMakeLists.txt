cmake_minimum_required(VERSION 3.18)

project(MyProject)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost 1.74.0 REQUIRED COMPONENTS beast)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  link_directories(${Boost_LIBRARY_DIRS})
  add_definitions(${Boost_DEFINITIONS})
endif()

add_executable(server server.cpp)
target_link_libraries(server Boost::beast)

add_executable(client client.cpp)
target_link_libraries(client Boost::beast)

target_link_libraries(MyProject PRIVATE Boost::beast)
