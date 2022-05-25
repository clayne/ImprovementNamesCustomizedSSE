cmake_minimum_required(VERSION 3.22)

project(
	ImprovementNamesCustomizedSSE
	VERSION 1.4.0
	LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
	message(FATAL_ERROR "in-source builds are not allowed")
endif()

include(cmake/common.cmake)

set_from_environment(Skyrim64Path)
if(NOT DEFINED Skyrim64Path)
	message(FATAL_ERROR "Skyrim64Path is not set")
endif()

add_subdirectory(src)
include(cmake/packaging.cmake)
