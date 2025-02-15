cmake_minimum_required(VERSION 3.10)

project(FOGLTOGLES VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES
	"translations/gles*/*.cpp"
)

set(HEADER_DIRS "translations" "utils")
file(GLOB_RECURSE HEADERS
	"translations/base/base.h"
	"utils/*.h"
)

set(DEPENDENCIES
	
)

message("Sources : ${SOURCES}")
message("Headers : ${HEADERS}")
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
