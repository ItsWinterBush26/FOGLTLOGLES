cmake_minimum_required(VERSION 3.11)

project(FOGLTLOGLES_tests LANGUAGES C CXX)

######## EARLY PROJECT SETTINGS ########
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(../) # root project

file(GLOB_RECURSE SOURCES
	"src/*.cpp"
)

set(HEADER_DIRS "include")
file(GLOB_RECURSE HEADERS
	"include/*.h"
    "include/*/*.h"
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(
	${PROJECT_NAME}
	PRIVATE
	${HEADER_DIRS}
)

target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC
    FOGLTLOGLES
)
