# CMakeList.txt : CMake project for Triangle, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project(Triangle)

# Add source to this project's executable.
add_executable (${PROJECT_NAME} "Triangle.cpp"  "phm_window.h" "phm_window.cpp" "phm_app.h" "phm_app.cpp" "phm_pipeline.h" "phm_pipeline.cpp")



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

add_custom_command(
         TARGET ${PROJECT_NAME} POST_BUILD
         COMMAND ${CMAKE_COMMAND} -E copy_directory              
             ${CMAKE_CURRENT_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
         COMMENT "Copying shaders" VERBATIM 
         )

# TODO: Add tests and install targets if needed.
