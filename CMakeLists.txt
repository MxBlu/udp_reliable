cmake_minimum_required(VERSION 3.6)
project(udp_reliable)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp UDPSocket.cpp UDPSocket.h Connection.cpp Connection.h)
add_executable(udp_reliable ${SOURCE_FILES})

if (WIN32)
    target_link_libraries(udp_reliable wsock32.lib)
    target_link_libraries(udp_reliable Ws2_32.lib)
endif()