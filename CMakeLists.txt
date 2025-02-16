cmake_minimum_required(VERSION 3.10)

project(FOGLTOGLES VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

set(SHADERC_SKIP_EXAMPLES ON)
set(SHADERC_SKIP_TESTS ON)
add_subdirectory(deps/shaderc)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_library(ANDROID_DEP android REQUIRED)
find_library(GLES2_DEP GLESv2 REQUIRED)
find_library(GLES3_DEP GLESv3 REQUIRED)
# find_library(SHADERC_DEP shaderc_static REQUIRED)
find_library(CXX_STATIC_DEP c++_static REQUIRED)

set(DEPENDENCIES
    ${ANDROID_DEP}
	${GLES2_DEP}
	${GLES3_DEP}
# 	${SHADERC_DEP}
	${CXX_STATIC_DEP}
)

file(GLOB_RECURSE SOURCES
	"src/wrappers/gles*/*.cpp"
)

set(HEADER_DIRS "include")
file(GLOB_RECURSE HEADERS
	"include/*/*.h"
)

message("Sources      : ${SOURCES}")
message("Headers      : ${HEADERS}")
message("Dependencies : ${DEPENDENCIES}")
add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

target_include_directories(
	${PROJECT_NAME}
	PUBLIC
	${HEADER_DIRS}
)

target_link_libraries(
	${PROJECT_NAME}
	${DEPENDENCIES}
)
