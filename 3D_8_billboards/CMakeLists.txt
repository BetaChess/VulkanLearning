# CMakeList.txt : CMake project for Triangle, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project(3D_8_billboards)

#message("${TEST}")

# Add source
set(
    SOURCES
    "3D.cpp" 
    "phm_model.h" 
    "phm_model.cpp" 
    "phm_window.h" 
    "phm_window.cpp" 
    "phm_swapchain.h" 
    "phm_swapchain.cpp" 
    "phm_app.h" 
    "phm_app.cpp" 
    "phm_pipeline.h" 
    "phm_pipeline.cpp" 
    "phm_device.h" 
    "phm_device.cpp" 
    "phm_model.h" 
    "phm_model.cpp" 
    "phm_object.h" 
    "phm_renderer.h" 
    "phm_renderer.cpp" 
    "time.h" 
    "time.cpp" 
    "phm_camera.h" 
    "phm_camera.cpp" 
    "phm_keyboardController.h" 
    "phm_keyboardController.cpp" 
    "pch.h" 
    "phm_utils.h" 
    "phm_object.cpp" 
    "phm_buffer.h" 
    "phm_buffer.cpp"  
    "phm_frame_info.h" 
    "phm_descriptor.h" 
    "phm_descriptor.cpp"
    "point_light_system.cpp"
	"simple_render_system.cpp"
    "point_light_system.h"
	"simple_render_system.h"
)

# Add the executable
message("${SOURCES}")
add_executable (${PROJECT_NAME} ${SOURCES})

# Add source groups
source_group("Render Systems" FILES 
    "point_light_system.cpp"
	"simple_render_system.cpp"
    "point_light_system.h"
	"simple_render_system.h"
)

source_group("Input" FILES
    "phm_keyboardController.cpp"
    "phm_keyboardController.h"
)

source_group("Vulkan" FILES
    "phm_swapchain.h" 
    "phm_swapchain.cpp" 
    "phm_pipeline.h" 
    "phm_pipeline.cpp" 
    "phm_device.h" 
    "phm_device.cpp" 
    "phm_buffer.h" 
    "phm_buffer.cpp"  
    "phm_descriptor.h" 
    "phm_descriptor.cpp"
)

source_group("Engine" FILES
    "phm_camera.cpp"
    "phm_camera.h"
    "phm_model.cpp"
    "phm_model.h"
    "phm_object.cpp"
    "phm_object.h"
    "phm_renderer.cpp"
    "phm_renderer.h"
    "phm_frame_info.h"
    "time.cpp"
    "time.h"
)

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

# Command to copy models to output folder
add_custom_command(
         TARGET ${PROJECT_NAME} POST_BUILD
         COMMAND ${CMAKE_COMMAND} -E copy_directory              
             ${CMAKE_CURRENT_SOURCE_DIR}/../models $<TARGET_FILE_DIR:${PROJECT_NAME}>/models
         COMMENT "Copying models" VERBATIM 
         )

# Adding debug PDB
set_property(
    TARGET ${PROJECT_NAME}
    APPEND PROPERTY
        CMAKE_COMPILE_PDB_OUTPUT_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>)

# Precompiled header
target_precompile_headers(${PROJECT_NAME}
    PUBLIC "pch.h"
)
# TODO: Add tests and install targets if needed.

############## Build SHADERS #######################
 
# Find all vertex and fragment sources within shaders directory
# taken from VBlancos vulkan tutorial
# https://github.com/vblanco20-1/vulkan-guide/blob/all-chapters/CMakeLists.txt
find_program(GLSL_VALIDATOR glslangValidator HINTS 
  ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE} 
  /usr/bin 
  /usr/local/bin 
  ${VULKAN_SDK_PATH}/Bin
  ${VULKAN_SDK_PATH}/Bin32
  $ENV{VULKAN_SDK}/Bin/ 
  $ENV{VULKAN_SDK}/Bin32/
)
 
# get all .vert and .frag files in shaders directory
file(GLOB_RECURSE GLSL_SOURCE_FILES
  "${PROJECT_SOURCE_DIR}/shaders/*.frag"
  "${PROJECT_SOURCE_DIR}/shaders/*.vert"
)

message("${GLSL_SOURCE_FILES}")
 
foreach(GLSL ${GLSL_SOURCE_FILES})
  get_filename_component(FILE_NAME ${GLSL} NAME)
  set(SPIRV "${PROJECT_SOURCE_DIR}/shaders/${FILE_NAME}.spv")
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
    DEPENDS ${GLSL})
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)
 
add_custom_target(
    ${PROJECT_NAME}_Shaders
    SOURCES ${GLSL_SOURCE_FILES}
    DEPENDS ${SPIRV_BINARY_FILES}
)

add_custom_command(
    TARGET ${PROJECT_NAME}_Shaders POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory              
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders ${CMAKE_CURRENT_BINARY_DIR}/shaders
    COMMENT "Copying shaders" VERBATIM 
    )

ADD_DEPENDENCIES(${PROJECT_NAME} ${PROJECT_NAME}_Shaders)