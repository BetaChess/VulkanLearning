# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("VulkanLearning")


if (WIN32)
	add_compile_definitions("PLATFORM_WINDOWS")
endif()

# Include sub-projects.
add_subdirectory ("Triangle")
add_subdirectory ("vendor/glfw")
add_subdirectory ("vendor/glm")

