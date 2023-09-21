#include "pch.h"

#include "phm_app.h"

#include "phm_keyboardController.h"
#include "phm_camera.h"
#include "simple_render_system.h"
#include "point_light_system.h"
#include "time.h"

#include "phm_manager.h"

#include "phm_pointLightComponent.h"
#include "phm_functionComponent.h"


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <iostream>
#include <cmath>
#include <math.h>


namespace phm
{
	

	Application::Application() 
	{
		loadObjects();
	}

	Application::~Application()
	{
	}

	void Application::run()
	{
		// Set up the camera/viewer
		Camera camera{};
		auto& viewerEntity = entityManager_.addEntity();
		
		entityManager_.setCamera(&camera);
		entityManager_.setViewerEntity(&viewerEntity);
		
		viewerEntity.transform.translation.z = -2.3f;
		viewerEntity.transform.translation.y = -0.5f;

		Time time;

		//float currRot = 0;

		while (!window_.shouldClose())
		{
			glfwPollEvents();

			time.updateTime();
			
			// BeginFrame returns a nullptr if the swapchain needs to be recreated. 
			// This skips the frame draw call, if that's the case.
			auto commandBuffer = renderer_.beginFrame();
			if (commandBuffer != nullptr)
			{
				const FrameInfo frameInfo{
					renderer_.getFrameIndex(),
					time.deltaTime(),
					commandBuffer,
					camera
				};

				// Update
				entityManager_.update(frameInfo, renderer_, window_.getGLFWWindow());
				
				// Render
				renderer_.beginSwapChainRenderPass(commandBuffer);
				entityManager_.render(frameInfo, renderer_);
				renderer_.endSwapChainRenderPass(commandBuffer);
				renderer_.endFrame();
			}
		}

		vkDeviceWaitIdle(device_.device());
	}


	void Application::loadObjects()
	{
		{
			auto& e = entityManager_.addEntity();
			e.addComponent<ecs::ModelComponent>(device_, "models/smooth_vase.obj");
			e.transform.translation = { -0.8f, 0.0f, 0.0f };
			e.transform.scale = glm::vec3(3);
		}
		{
			auto& e = entityManager_.addEntity();
			e.addComponent<ecs::ModelComponent>(device_, "models/flat_vase.obj");
			e.transform.translation = { 0.8f, 0.0f, 0.0f };
			e.transform.scale = glm::vec3(3);
		}
		{
			auto& e = entityManager_.addEntity();
			e.addComponent<ecs::ModelComponent>(device_, "models/quad.obj");
			e.transform.translation = { 0.0f, 0.0f, 0.0f };
			e.transform.scale = glm::vec3{ 3.0f, 1.0f, 3.0f };
		}
		{
			std::function rotateFunc = FUNCTIONCOMPONENTLAMDA(1, const int, lightOffset)
			{
				constexpr int numberOfLights = 4;
				constexpr float rotationSpeed = 0;

				e.transform.translation = 
				{
					2.0f * cos(glm::two_pi<float>() / numberOfLights * lightOffset[0] + std::fmod(Time::elapsedTime() * rotationSpeed, glm::two_pi<float>())),
					e.transform.translation.y, 
					2.0f * sin(glm::two_pi<float>() / numberOfLights * lightOffset[0] + std::fmod(Time::elapsedTime() * rotationSpeed, glm::two_pi<float>()))
				};
			};
			
			// Add the lights
			{
				auto& e = entityManager_.addEntity();
				e.addComponent<ecs::PointlightComponent>(glm::vec3(1.0f, 0.0f, 0.0f), 0.7f, 0.01f);
				e.transform.translation = { 1.3f, -1.0f, 1.3f };
				const std::array<const int, 1> arr = { 0 };
				e.addComponent<ecs::FunctionComponent<1>>(rotateFunc, arr);
			}
			{
				auto& e = entityManager_.addEntity();
				e.addComponent<ecs::PointlightComponent>(glm::vec3(0.0f, 0.0f, 1.0f), 0.4f, 0.1f);
				e.transform.translation = { -1.3f, -1.0f, -1.3f };
				const std::array<const int, 1> arr = { 1 };
				e.addComponent<ecs::FunctionComponent<1>>(rotateFunc, arr);
			}
			{
				auto& e = entityManager_.addEntity();
				e.addComponent<ecs::PointlightComponent>(glm::vec3(0.0f, 1.0f, 0.0f), 0.1f, 0.05f);
				e.transform.translation = { -1.3f, -1.0f, -1.3f };
				const std::array<const int, 1> arr = { 2 };
				e.addComponent<ecs::FunctionComponent<1>>(rotateFunc, arr);
			}
			{
				auto& e = entityManager_.addEntity();
				e.addComponent<ecs::PointlightComponent>(glm::vec3(1.0f, 1.0f, 1.0f), 0.34f, 0.05f);
				e.transform.translation = { -1.3f, -1.0f, -1.3f };
				const std::array<const int, 1> arr = { 3 };
				e.addComponent<ecs::FunctionComponent<1>>(rotateFunc, arr);
			}
		}
	}
}


