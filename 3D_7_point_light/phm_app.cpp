#include "pch.h"

#include "phm_app.h"

#include "phm_keyboardController.h"
#include "phm_camera.h"
#include "simple_render_system.h"
#include "time.h"


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <iostream>


namespace phm
{
	struct GlobalUbo
	{
		glm::mat4 projectionView{ 1.0f };
		glm::vec4 ambientLightColor{ 1.0f, 1.0f, 1.0f, 0.02f };
		glm::vec3 lightPosition{ -0.4f, -1.0f, -1.2f };
		alignas(16) glm::vec4 lightColor{ 1.0f, 1.0f, 1.0f, 0.7f };
	};

	Application::Application()
	{
		globalPool_ = DescriptorPool::Builder(device_)
			.setMaxSets(Swapchain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, Swapchain::MAX_FRAMES_IN_FLIGHT)
			.build();

		loadObjects();
	}

	Application::~Application()
	{
	}

	void Application::run()
	{
		std::vector<std::unique_ptr<Buffer>> uniformBuffers(Swapchain::MAX_FRAMES_IN_FLIGHT);
		for (auto& bufferPtr : uniformBuffers)
		{
			bufferPtr = std::make_unique<Buffer>(device_,
				sizeof(GlobalUbo),
				Swapchain::MAX_FRAMES_IN_FLIGHT,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
				device_.properties.limits.minUniformBufferOffsetAlignment);
			bufferPtr->map();
		}

		auto globalSetLayout = DescriptorSetLayout::Builder(device_)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();

		std::vector<VkDescriptorSet> globalDescriptorSets(Swapchain::MAX_FRAMES_IN_FLIGHT);
		for (size_t i = 0; i < globalDescriptorSets.size(); i++)
		{
			auto bufferInfo = uniformBuffers[i]->descriptorInfo();
			DescriptorWriter(*globalSetLayout, *globalPool_)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]);
		}

		SimpleRenderSystem simpleRenderSystem{ device_, renderer_.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };

		Camera camera{};
		Object viewerObject;
		viewerObject.transform.translation.z = -2.3f;
		viewerObject.transform.translation.y = -0.5f;
		KeyboardController cameraController{};

		Time time;

		while (!window_.shouldClose())
		{
			glfwPollEvents();

			time.updateTime();

			cameraController.moveInPlaneXZ(window_.getGLFWWindow(), time.deltaTime(), viewerObject);
			camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

			float aspect = renderer_.getAspectRatio();
			camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 100.0f);
			
			// BeginFrame returns a nullptr if the swapchain needs to be recreated. 
			// This skips the frame draw call, if that's the case.
			auto commandBuffer = renderer_.beginFrame();
			if (commandBuffer != nullptr)
			{
				const FrameInfo frameInfo{
					renderer_.getFrameIndex(),
					time.deltaTime(),
					commandBuffer,
					camera,
					globalDescriptorSets[renderer_.getFrameIndex()]
				};

				// Update
				GlobalUbo ubo{};
				ubo.projectionView = camera.getProjection() * camera.getView();
				uniformBuffers[frameInfo.frameIndex]->writeToBuffer(&ubo);
				uniformBuffers[frameInfo.frameIndex]->flush();

				// Render
				renderer_.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderObjects(frameInfo, objects_);
				renderer_.endSwapChainRenderPass(commandBuffer);
				renderer_.endFrame();
			}
		}

		vkDeviceWaitIdle(device_.device());
	}


	void Application::loadObjects()
	{
		{
			std::shared_ptr<Model> model = Model::createModelFromFile(device_, "models/smooth_vase.obj");

			Object object;
			object.model = model;
			object.transform.translation = { -0.8f, 0.0f, 0.0f };
			object.transform.scale = glm::vec3(3);
			//object.transform.rotation.x = 0.1f * glm::two_pi<float>();

			objects_.push_back(std::move(object));
		}
		{
			std::shared_ptr<Model> model = Model::createModelFromFile(device_, "models/flat_vase.obj");

			Object object;
			object.model = model;
			object.transform.translation = { 0.8f, 0.0f, 0.0f };
			object.transform.scale = glm::vec3(3);
			//object.transform.rotation.x = 0.1f * glm::two_pi<float>();

			objects_.push_back(std::move(object));
		}
		{
			std::shared_ptr<Model> model = Model::createModelFromFile(device_, "models/quad.obj");

			Object object;
			object.model = model;
			object.transform.translation = { 0.0f, 0.0f, 0.0f };
			object.transform.scale = glm::vec3{ 3.0f, 1.0f, 3.0f };
			//object.transform.rotation.x = 0.1f * glm::two_pi<float>();

			objects_.push_back(std::move(object));
		}
	}
}


