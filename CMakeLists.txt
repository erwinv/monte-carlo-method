cmake_minimum_required(VERSION 3.21)

project(monte-carlo LANGUAGES CXX)

add_executable(pi_s pi_singlethread.cpp)

add_executable(pi pi.cpp)
set_property(TARGET pi PROPERTY CXX_STANDARD 17)
if (CMAKE_HOST_WIN32)
  target_include_directories(pi PUBLIC "C:/Users/erwin/source/repos/xdispatch/include")
  target_link_directories(pi PUBLIC "C:/Users/erwin/source/repos/xdispatch/lib")
  add_custom_command(TARGET pi POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "C:/Users/erwin/source/repos/xdispatch/bin/dispatch.dll" $<TARGET_FILE_DIR:pi>
    COMMAND_EXPAND_LISTS
  )
endif()
target_link_libraries(pi LINK_PUBLIC dispatch)

if (CMAKE_HOST_WIN32)
    add_executable(pi_ms_ppl pi_microsoft_ppl.cpp)
endif()