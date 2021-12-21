# CMakeList.txt : CMake project for Triangle, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project(3D_uniform)

# Add source to this project's executable.
add_executable (${PROJECT_NAME} "3D.cpp" "phm_model.h" "phm_model.cpp" "phm_window.h" "phm_window.cpp" "phm_swapchain.h" "phm_swapchain.cpp" "phm_app.h" "phm_app.cpp" "phm_pipeline.h" "phm_pipeline.cpp" "phm_device.h" "phm_device.cpp" "phm_model.h" "phm_model.cpp" "phm_object.h" "phm_renderer.h" "phm_renderer.cpp" "simple_render_system.h" "simple_render_system.cpp" "time.h" "time.cpp" "phm_camera.h" "phm_camera.cpp" "phm_keyboardController.h" "phm_keyboardController.cpp" "pch.h" "phm_utils.h" "phm_object.cpp" "phm_buffer.h" "phm_buffer.cpp"  "phm_frame_info.h" "phm_descriptor.h" "phm_descriptor.cpp")


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
    PUBLIC ../vendor/tinyobjloader/
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

add_custom_command(
         TARGET ${PROJECT_NAME} POST_BUILD
         COMMAND ${CMAKE_COMMAND} -E copy_directory              
             ${CMAKE_CURRENT_SOURCE_DIR}/../models $<TARGET_FILE_DIR:${PROJECT_NAME}>/models
         COMMENT "Copying models" VERBATIM 
         )

set_property(
    TARGET ${PROJECT_NAME}
    APPEND PROPERTY
        CMAKE_COMPILE_PDB_OUTPUT_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>)

target_precompile_headers(${PROJECT_NAME}
    PRIVATE "pch.h"
)
# TODO: Add tests and install targets if needed.
