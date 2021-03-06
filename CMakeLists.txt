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

add_compile_options("/wd26812")

add_compile_options("$<$<NOT:$<CONFIG:Debug>>:/Zi>")
add_link_options("$<$<NOT:$<CONFIG:Debug>>:/DEBUG>")
add_link_options("$<$<NOT:$<CONFIG:Debug>>:/OPT:REF>")
add_link_options("$<$<NOT:$<CONFIG:Debug>>:/OPT:ICF>")

# Include sub-projects.
add_subdirectory ("vendor/glfw")
add_subdirectory ("vendor/glm")

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