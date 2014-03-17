project(Server-Client)
cmake_minimum_required(VERSION 2.8)

find_package(Threads)

add_executable(client client.cpp)
target_link_libraries(client ${CMAKE_THREAD_LIBS_INIT})

add_executable(server-st server-single.cpp)
target_link_libraries(server-st ${CMAKE_THREAD_LIBS_INIT})

add_executable(server-mt server-multi.cpp)
target_link_libraries(server-mt ${CMAKE_THREAD_LIBS_INIT})
