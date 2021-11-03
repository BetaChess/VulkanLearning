# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("VulkanLearning")


if (WIN32)
	add_compile_definitions("PLATFORM_WINDOWS")
endif()

if (CMAKE_BUILD_TYPE MATCHES "Debug_with_info")
	add_compile_definitions("DEBUGWITHINFO")
endif()


# Include sub-projects.
add_subdirectory ("Triangle_1")
add_subdirectory ("Triangle_2_pushconstants")
add_subdirectory ("Triangle_3_restructured")
add_subdirectory ("Triangle_4_homogeneous")
<<<<<<< HEAD
add_subdirectory ("3D_1")
=======
>>>>>>> 36771473584509ef4e0550e17a1c205503f0cd01
add_subdirectory ("vendor/glfw")
add_subdirectory ("vendor/glm")

