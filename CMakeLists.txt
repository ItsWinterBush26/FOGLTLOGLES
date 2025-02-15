cmake_minimum_required(VERSION 3.10)

project(FOGLTOGLES VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++")

file(GLOB_RECURSE SOURCES
	"src/wrappers/gles*/*.cpp"
)

set(HEADER_DIRS "include")
file(GLOB_RECURSE HEADERS
	"include/*/*.h"
)

message("Sources : ${SOURCES}")
message("Headers : ${HEADERS}")
add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

target_include_directories(
	${PROJECT_NAME}
	PUBLIC
	${HEADER_DIRS}
)

find_library(GLES2_DEP GLESv2 REQUIRED)
find_library(GLES3_DEP GLESv3 REQUIRED)

set(DEPENDENCIES
	${GLES2_DEP}
	${GLES3_DEP}
)

target_link_libraries(
	${PROJECT_NAME}
	${DEPENDENCIES}
)
