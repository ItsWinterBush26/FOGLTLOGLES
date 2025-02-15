cmake_minimum_required(VERSION 3.10)
project(FOGLTOGLES VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    src/layer.cpp
    translations/gles20/direct.cpp
    translations/gles20/emulated.cpp
    translations/gles30/glclientwaitsync.cpp
    translations/gles30/glfencesync.cpp
    translations/gles30/glwaitsync.cpp
    translations/gles30/gldeletesync.cpp
)

add_library(FOGLTOGLES SHARED ${SOURCES})
target_include_directories(FOGLTOGLES PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/translations
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
)

find_package(Threads REQUIRED)
find_library(GLESv3_LIB GLESv3)
find_library(GLESv2_LIB GLESv2)
find_library(EGL_LIB EGL)
find_library(ANDROID_LIB android)
find_library(LOG_LIB log)
find_library(C_LIB c)
find_library(CXX_STATIC_LIB c++_static)

target_link_libraries(FOGLTOGLES PRIVATE
    Threads::Threads
    ${GLESv3_LIB}
    ${GLESv2_LIB}
    ${EGL_LIB}
    ${ANDROID_LIB}
    ${LOG_LIB}
    ${C_LIB}
    ${CXX_STATIC_LIB}
)

