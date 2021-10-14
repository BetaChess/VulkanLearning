# CMakeList.txt : CMake project for Triangle, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project(Triangle_restructured)

# Add source to this project's executable.
add_executable (${PROJECT_NAME} "TriangleRestructured.cpp" "phm_model.h" "phm_model.cpp" "phm_window.h" "phm_window.cpp" "phm_swapchain.h" "phm_swapchain.cpp" "phm_app.h" "phm_app.cpp" "phm_pipeline.h" "phm_pipeline.cpp" "phm_device.h" "phm_device.cpp" "phm_model.h" "phm_model.cpp" "phm_object.h" "phm_renderer.h" "phm_renderer.cpp" "simple_render_system.h" "simple_render_system.cpp")

set_target_properties(${PROJECT_NAME} 
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

target_include_directories(${PROJECT_NAME} 
	PUBLIC $ENV{VULKAN_SDK}/Include/ 
	PUBLIC ../vendor/glfw/include/
    PUBLIC ../vendor/glm/
)

target_link_directories(${PROJECT_NAME}
	PRIVATE ../vendor/glfw/src
    PRIVATE $ENV{VULKAN_SDK}/Lib
)
 
target_link_libraries(${PROJECT_NAME}
	glfw
    vulkan-1
)

add_custom_command(
         TARGET ${PROJECT_NAME} POST_BUILD
         COMMAND ${CMAKE_COMMAND} -E copy_directory              
             ${CMAKE_CURRENT_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
         COMMENT "Copying shaders" VERBATIM 
         )

# TODO: Add tests and install targets if needed.
