cmake_minimum_required(VERSION 3.10)
project(raskol)

set(CMAKE_CXX_STANDARD 14)

find_package(PkgConfig REQUIRED)
pkg_check_modules(PORTAUDIO REQUIRED portaudio-2.0)

include_directories(${PORTAUDIO_INCLUDE_DIRS})

add_executable(main main.cpp)

target_link_libraries(main ${PORTAUDIO_LIBRARIES})
