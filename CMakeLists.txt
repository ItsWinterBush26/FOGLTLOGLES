cmake_minimum_required(VERSION 3.10)
project(FOGLTOGLES VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB GLES20_FILES "${CMAKE_CURRENT_SOURCE_DIR}/translations/gles20/*.cpp")
file(GLOB GLES30_FILES "${CMAKE_CURRENT_SOURCE_DIR}/translations/gles30/*.cpp")

set(SOURCES
    ${SRC_FILES}
    ${GLES20_FILES}
    ${GLES30_FILES}
)

add_library(FOGLTOGLES SHARED ${SOURCES})

target_include_directories(FOGLTOGLES PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/translations"
    "${CMAKE_CURRENT_SOURCE_DIR}/utils"
)

find_package(Threads REQUIRED)
find_library(GLES3_LIB GLESv3 REQUIRED)
find_library(GLES2_LIB GLESv2 REQUIRED)
find_library(EGL_LIB EGL REQUIRED)
find_library(ANDROID_LIB android)
find_library(LOG_LIB log)
find_library(C_LIB c)
find_library(CXX_STATIC_LIB c++_static)

target_link_libraries(FOGLTOGLES PRIVATE
    Threads::Threads
    ${GLES3_LIB}
    ${GLES2_LIB}
    ${EGL_LIB}
    ${ANDROID_LIB}
    ${LOG_LIB}
    ${C_LIB}
    ${CXX_STATIC_LIB}
)

