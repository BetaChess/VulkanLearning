﻿# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("VulkanLearning")

if (WIN32)
	add_compile_definitions("PLATFORM_WINDOWS")
endif()

if (DEBUGWITHINFO)
	add_compile_definitions("DEBUGWITHINFO")
	add_compile_definitions("DEBUGADDITIONAL")
endif()

if (DEBUGADDITIONAL)
	add_compile_definitions("DEBUGADDITIONAL")
endif()

# Include sub-projects.
add_subdirectory ("vendor/glfw")
add_subdirectory ("vendor/glm")

add_subdirectory ("CheckSupport")
add_subdirectory ("Triangle_1")
add_subdirectory ("Triangle_2_pushconstants")
add_subdirectory ("Triangle_3_restructured")
add_subdirectory ("Triangle_4_homogeneous")
add_subdirectory ("3D_1")
add_subdirectory ("3D_2_perspective")
add_subdirectory ("3D_3_input")
add_subdirectory ("3D_4_index_buffers")
add_subdirectory ("3D_5_directional_vertex_light")
add_subdirectory ("3D_6_uniform")
add_subdirectory ("3D_7_point_light")
add_subdirectory ("3D_8_billboards")
add_subdirectory ("3D_9_specular_lighting")
add_subdirectory ("Showcase_1_projection_showcase")

# Find the libraries using https://github.com/SaschaWillems/Vulkan/blob/master/CMakeLists.txt
# Note: MIT License
# Use FindVulkan module added with CMAKE 3.7
if (NOT CMAKE_VERSION VERSION_LESS 3.7.0)
	message(STATUS "Using module to find Vulkan")
	find_package(Vulkan)
endif()
IF(UNIX AND NOT APPLE)
	set(LINUX TRUE)
ENDIF()

IF(WIN32)
	IF (NOT Vulkan_FOUND)
		find_library(Vulkan_LIBRARY NAMES vulkan-1 vulkan PATHS ${CMAKE_SOURCE_DIR}/libs/vulkan)
		IF (Vulkan_LIBRARY)
			set(Vulkan_FOUND ON)
			MESSAGE("Using bundled Vulkan library version")
		ENDIF()
	ENDIF()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_WIN32_KHR")
ELSEIF(LINUX)
	IF (NOT Vulkan_FOUND)
		find_library(Vulkan_LIBRARY NAMES vulkan HINTS "$ENV{VULKAN_SDK}/lib" "${CMAKE_SOURCE_DIR}/libs/vulkan" REQUIRED)
		IF (Vulkan_LIBRARY)
			set(Vulkan_FOUND ON)
			MESSAGE("Using bundled Vulkan library version")
		ENDIF()
	ENDIF()
ENDIF(WIN32)
