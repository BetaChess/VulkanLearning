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
#include <math.h>
#include <cmath>


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
		//std::vector<std::unique_ptr<Buffer>> uniformBuffers(Swapchain::MAX_FRAMES_IN_FLIGHT);
		/*for (auto& bufferPtr : uniformBuffers)
		{
			bufferPtr = std::make_unique<Buffer>(device_,
				sizeof(GlobalUbo),
				Swapchain::MAX_FRAMES_IN_FLIGHT,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
				device_.properties.limits.minUniformBufferOffsetAlignment);
			bufferPtr->map();
		}*/

		/*auto globalSetLayout = DescriptorSetLayout::Builder(device_)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();

		std::vector<VkDescriptorSet> globalDescriptorSets(Swapchain::MAX_FRAMES_IN_FLIGHT);
		for (size_t i = 0; i < globalDescriptorSets.size(); i++)
		{
			auto bufferInfo = uniformBuffers[i]->descriptorInfo();
			DescriptorWriter(*globalSetLayout, *globalPool_)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]);
		}*/

		//SimpleRenderSystem simpleRenderSystem{ device_, renderer_.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		//PointLightSystem pointLightSystem{ device_, renderer_.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };

		// Make the ECS manager
		//ecs::Manager entityManager{ device_, renderer_.getSwapChainRenderPass(), globalPool_.get() };
		
		// Set up the camera/viewer
		Camera camera{};
		auto& viewerEntity = entityManager_.addEntity();
		
		entityManager_.setCamera(&camera);
		entityManager_.setViewerEntity(&viewerEntity);
		
		viewerEntity.transform.translation.z = -2.3f;
		viewerEntity.transform.translation.y = -0.5f;
		
		//Object viewerObject;
		//viewerObject.transform.translation.z = -2.3f;
		//viewerObject.transform.translation.y = -0.5f;
		/*KeyboardController cameraController{};*/

		

		Time time;

		//float currRot = 0;

		while (!window_.shouldClose())
		{
			glfwPollEvents();

			time.updateTime();

			//cameraController.moveInPlaneXZ(window_.getGLFWWindow(), time.deltaTime(), viewerObject);
			//camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

			/*float aspect = renderer_.getAspectRatio();
			camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 100.0f);*/
			
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
				//currRot += glm::two_pi<float>() / 3.0f * time.deltaTime();

				/*GlobalUbo ubo{};
				ubo.projection = camera.getProjection();
				ubo.view = camera.getView();
				ubo.lightPosition = { cos(currRot) * 1.3f, -1.0f, sin(currRot) * 1.3f };
				uniformBuffers[frameInfo.frameIndex]->writeToBuffer(&ubo);
				uniformBuffers[frameInfo.frameIndex]->flush();*/

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
				constexpr int numberOfLights = 3;
				constexpr float rotationSpeed = 1.3f;

				e.transform.translation = 
				{
					1.3f * cos(glm::two_pi<float>() / numberOfLights * lightOffset[0] + std::fmodf(Time::elapsedTime() * rotationSpeed, glm::two_pi<float>())),
					e.transform.translation.y, 
					1.3f * sin(glm::two_pi<float>() / numberOfLights * lightOffset[0] + std::fmodf(Time::elapsedTime() * rotationSpeed, glm::two_pi<float>()))
				};
			};
			
			{
				auto& e = entityManager_.addEntity();
				e.addComponent<ecs::PointlightComponent>(glm::vec3(1.0f, 0.0f, 0.0f), 1.5f, 0.5f);
				e.transform.translation = { 1.3f, -1.0f, 1.3f };
				const std::array<const int, 1> arr = { 0 };
				e.addComponent<ecs::FunctionComponent<1>>(rotateFunc, arr);
			}
			{
				auto& e = entityManager_.addEntity();
				e.addComponent<ecs::PointlightComponent>(glm::vec3(0.0f, 0.0f, 1.0f), 0.5f, 0.1f);
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
		}
	}
}


