cmake_minimum_required(VERSION 3.0)
project(rayTracer)

include(CheckCXXCompilerFlag)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14")
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

include_directories("include")
file(GLOB_RECURSE HEADERS "include/*.h" "include/*.hpp")
file(GLOB_RECURSE SOURCES "sources/*.cpp")

#enkiTS (scheduler)
INCLUDE_DIRECTORIES("extern/enkiTS/src")
LINK_DIRECTORIES("extern/enkiTS/build")


add_executable(${CMAKE_PROJECT_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${CMAKE_PROJECT_NAME} SDL2 pthread enkiTS)
if(UNIX)
        set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
        find_package(Threads REQUIRED)
        if(CMAKE_USE_PTHREADS_INIT)
                set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
        endif()
        target_link_libraries (${CMAKE_PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
endif()

