cmake_minimum_required (VERSION 3.8)

project(CheckSupport)

# Add source to this project's executable.
add_executable (${PROJECT_NAME} "main.cpp")

set_target_properties(${PROJECT_NAME}
	PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED YES
	CXX_EXTENSIONS NO
	)

target_include_directories(${PROJECT_NAME}
	PUBLIC $ENV{VULKAN_SDK}/Include/
	PUBLIC ../vendor/glfw/include/
	PUBLIC ../vendor/glad/include/
	)

target_link_directories(${PROJECT_NAME}
	PRIVATE ../vendor/glfw/src
	#PRIVATE $ENV{VULKAN_SDK}/Lib
	)

target_link_libraries(${PROJECT_NAME}
	glfw
	${Vulkan_LIBRARY}
	)