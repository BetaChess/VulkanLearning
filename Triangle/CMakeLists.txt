# CMakeList.txt : CMake project for Triangle, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project(Triangle)

# Add source to this project's executable.
add_executable (${PROJECT_NAME} "Triangle.cpp" )



target_include_directories(${PROJECT_NAME} 
	PUBLIC $ENV{VULKAN_SDK}/Include/ 
	PUBLIC ../vendor/glfw/include/
)

target_link_directories(${PROJECT_NAME}
	PRIVATE ../vendor/glfw/src
)

target_link_libraries(${PROJECT_NAME}
	glfw
)

# TODO: Add tests and install targets if needed.
