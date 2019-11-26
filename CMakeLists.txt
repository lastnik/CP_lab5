cmake_minimum_required(VERSION 3.14)
project(ECDSA)

set (CMAKE_CXX_FLAGS "-fexceptions -Wall")

set(CMAKE_CXX_STANDARD 17)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/scripts")
include(DirectorySeacher)
set(openSSL "C:/OpenSSL")
include_directories(ECDSA ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(ECDSA ${CMAKE_CURRENT_SOURCE_DIR}/BigInt)
include_directories(ECDSA ${CMAKE_CURRENT_SOURCE_DIR}/Utilits)
include_directories(ECDSA ${CMAKE_CURRENT_SOURCE_DIR}/ElipticCurve)
include_directories(ECDSA ${openSSL}/include)
add_dir_to_executable(ECDSA ${CMAKE_CURRENT_SOURCE_DIR})
dir_to_target_sources(ECDSA ${CMAKE_CURRENT_SOURCE_DIR}/BigInt)
dir_to_target_sources(ECDSA ${CMAKE_CURRENT_SOURCE_DIR}/Utilits)
dir_to_target_sources(ECDSA ${CMAKE_CURRENT_SOURCE_DIR}/ElipticCurve)


target_link_libraries(ECDSA ${openSSL}/lib/libssl.a ${openSSL}/lib/libcrypto.a)