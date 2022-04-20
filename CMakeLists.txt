cmake_minimum_required(VERSION 3.23)

project(monte-carlo LANGUAGES CXX)

add_executable(pi pi.cpp)
set_property(TARGET pi PROPERTY CXX_STANDARD 23)
target_link_libraries(pi LINK_PUBLIC dispatch)

add_executable(pi_s pi_singlethread.cpp)
