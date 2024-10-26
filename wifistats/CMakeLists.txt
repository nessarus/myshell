cmake_minimum_required(VERSION 3.10)
project(wifistats)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

file(GLOB SOURCES "*.c")
file(GLOB HEADERS "*.h")

add_executable(wifistats ${SOURCES} ${HEADERS})
