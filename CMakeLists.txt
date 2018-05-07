cmake_minimum_required(VERSION 3.0)
project(rayTracer)

include(CheckCXXCompilerFlag)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14")

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

if(CMAKE_BUILD_TYPE MATCHES Release)
    message(${CMAKE_BUILD_TYPE})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
endif(CMAKE_BUILD_TYPE MATCHES Release)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")
find_package(SDL2TTF REQUIRED)
include_directories(${SDL2TTF_INCLUDE_DIRS})

include_directories("include")
file(GLOB_RECURSE HEADERS "include/*.h" "include/*.hpp")
file(GLOB_RECURSE SOURCES "sources/*.cpp")

INCLUDE_DIRECTORIES("extern/image")

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${CMAKE_PROJECT_NAME} SDL2 ${SDL2TTF_LIBRARY} pthread)


if(UNIX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse3")
        set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
        find_package(Threads REQUIRED)
        if(CMAKE_USE_PTHREADS_INIT)
                set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
        endif()
        target_link_libraries (${CMAKE_PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
endif()

add_custom_command(
        TARGET rayTracer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/Resources/arial.ttf
                ${CMAKE_CURRENT_BINARY_DIR}/Resources/arial.ttf)

add_custom_command(
        TARGET rayTracer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/Resources/ground.jpg
                ${CMAKE_CURRENT_BINARY_DIR}/Resources/ground.jpg)

