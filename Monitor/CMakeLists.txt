cmake_minimum_required(VERSION 3.26.5)

project(Monitor VERSION 1.0.0)

find_package(ROOT REQUIRED)
include(${ROOT_USE_FILE})

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${ROOT_INCLUDE_DIRS})

file(GLOB HEADERS ${PROJECT_SOURCE_DIR}/include/*.h)
file(GLOB SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_executable(Monitor main.cpp ${HEADERS} ${SOURCES})
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
target_link_libraries(Monitor ${ROOT_LIBRARIES})
