cmake_minimum_required(VERSION 3.23)

project(
	CloudsUpgrade
	VERSION 1.0.0
	LANGUAGES CXX
)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(NVSEPlugin)

find_path(SIMPLEINI_INCLUDE_DIRS "ConvertUTF.c")

target_include_directories(
	"${PROJECT_NAME}"
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/include
	PRIVATE
		${CMAKE_CURRENT_BINARY_DIR}/cmake
		${CMAKE_CURRENT_SOURCE_DIR}/src
		${CMAKE_CURRENT_SOURCE_DIR}/external/JIP-LN-NVSE
		${SIMPLEINI_INCLUDE_DIRS}
)

find_package(fmt CONFIG REQUIRED)

target_link_libraries(
	${PROJECT_NAME} 
	PRIVATE
		fmt::fmt
		debug ${CMAKE_CURRENT_BINARY_DIR}/vcpkg_installed/x86-windows-static-md/debug/lib/detours.lib
		optimized ${CMAKE_CURRENT_BINARY_DIR}/vcpkg_installed/x86-windows-static-md/lib/detours.lib
)
