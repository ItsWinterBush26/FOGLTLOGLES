cmake_minimum_required(VERSION 3.10)

project(FOGLTOGLES LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

######## DEPENDENCIES ########
set(SHADERC_SKIP_EXAMPLES ON)
set(SHADERC_SKIP_TESTS ON)
set(SPIRV_SKIP_TESTS ON)
set(SPIRV_CROSS_ENABLE_TESTS OFF)
set(SPIRV_CROSS_ENABLE_HLSL OFF)
set(SPIRV_CROSS_ENABLE_MSL OFF)
set(SPIRV_CROSS_CLI OFF)
set(SPIRV_CROSS_ENABLE_C_API OFF)
set(GLSLANG_TESTS OFF)
## set(ENABLE_HLSL OFF) ## fails on libshaderc_util

add_subdirectory(deps/shaderc) 
add_subdirectory(deps/spirv-cross)

######## MAIN PROJECT ########
file(GLOB_RECURSE SOURCES
	"src/*.cpp"
	"src/*/*.cpp"
	"src/*/*/*.cpp"
)

set(HEADER_DIRS "include" "build_info")
file(GLOB_RECURSE HEADERS
	"include/*.h"
	"include/*/*.h"
	"include/*/*/*'h"
	"build_info/build_info.h"
)

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})
set_target_properties(
	${PROJECT_NAME}
	PROPERTIES
	CXX_VISIBILITY_PRESET hidden
	C_VISIBILITY_PRESET hidden
)

target_include_directories(
	${PROJECT_NAME}
	PUBLIC
	${HEADER_DIRS}
)

######## EXTRA ########
if(CMAKE_BUILD_TYPE MATCHES Release)
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_STRIP} $<TARGET_FILE:${PROJECT_NAME}>
	COMMENT "Stripping symbols from ${PROJECT_NAME}")
endif()

######## DEPENDENCIES ########
find_library(ANDROID_DEP android REQUIRED)
find_library(EGL_DEP EGL REQUIRED)
find_library(GLES1_DEP GLESv1_CM REQUIRED)
find_library(GLES2_DEP GLESv2 REQUIRED)
find_library(GLES3_DEP GLESv3 REQUIRED)
find_library(LOG_DEP log REQUIRED)
find_library(DL_DEP dl REQUIRED)
find_library(CXX_STATIC_DEP c++_static REQUIRED)

set(DEPENDENCIES
	${ANDROID_DEP}
	${EGL_DEP}
	${GLES1_DEP}
	${GLES2_DEP}
	${GLES3_DEP}
	${LOG_DEP}
	${DL_DEP}
	${CXX_STATIC_DEP}

	# Static ShaderC
	shaderc

	# Minimal static SPIRV-Cross
	spirv-cross-core
	spirv-cross-glsl
)

# set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_OPTIONS -fopenmp)
target_link_libraries(
	${PROJECT_NAME} PRIVATE ${DEPENDENCIES}
)

message("Sources      : ${SOURCES}")
message("Headers      : ${HEADERS}")
message("Dependencies : ${DEPENDENCIES}")

set(BUILD_INFO_DIR ${CMAKE_SOURCE_DIR}/build_info)

add_custom_target(generateBuildInfo
  ${CMAKE_COMMAND} -D SRC=${BUILD_INFO_DIR}/build_info.h.in
                   -D DST=${BUILD_INFO_DIR}/build_info.h
                   -D GIT_EXECUTABLE=${GIT_EXECUTABLE}
                   -P ${BUILD_INFO_DIR}/GenerateBuildInfo.cmake
)

add_dependencies(${PROJECT_NAME} generateBuildInfo)