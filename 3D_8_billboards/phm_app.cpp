#include "pch.h"

#include "phm_app.h"

#include "phm_keyboardController.h"
#include "phm_camera.h"
#include "simple_render_system.h"
#include "point_light_system.h"
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
		glm::mat4 projection{ 1.0f };
		glm::mat4 view{ 1.0f };
		glm::vec4 ambientLightColor{ 1.0f, 1.0f, 1.0f, 0.02f };
		glm::vec3 lightPosition{ -0.4f, -1.0f, -1.2f };
		alignas(16) glm::vec4 lightColor{ 0.29019607843137254901960784313725f, 0.25490196078431372549019607843137f, 0.16470588235294117647058823529412f, 0.7f };
	};

	Application::Application()
	{
		globalPool_ = PhmDescriptorPool::Builder(device_)
			.setMaxSets(PhmSwapchain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, PhmSwapchain::MAX_FRAMES_IN_FLIGHT)
			.build();

		loadObjects();
	}

	Application::~Application()
	{
	}

	void Application::run()
	{
		std::vector<std::unique_ptr<PhmBuffer>> uniformBuffers(PhmSwapchain::MAX_FRAMES_IN_FLIGHT);
		for (auto& bufferPtr : uniformBuffers)
		{
			bufferPtr = std::make_unique<PhmBuffer>(device_,
				sizeof(GlobalUbo),
				PhmSwapchain::MAX_FRAMES_IN_FLIGHT,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
				device_.properties.limits.minUniformBufferOffsetAlignment);
			bufferPtr->map();
		}

		auto globalSetLayout = PhmDescriptorSetLayout::Builder(device_)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();

		std::vector<VkDescriptorSet> globalDescriptorSets(PhmSwapchain::MAX_FRAMES_IN_FLIGHT);
		for (size_t i = 0; i < globalDescriptorSets.size(); i++)
		{
			auto bufferInfo = uniformBuffers[i]->descriptorInfo();
			PhmDescriptorWriter(*globalSetLayout, *globalPool_)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]);
		}

		SimpleRenderSystem simpleRenderSystem{ device_, renderer_.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		PointLightSystem pointLightSystem{ device_, renderer_.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };

		PhmCamera camera{};
		PhmObject viewerObject;
		viewerObject.transform.translation.z = -2.3f;
		viewerObject.transform.translation.y = -0.5f;
		PhmKeyboardController cameraController{};

		Time time;

		float currRot = 0;

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
				currRot += glm::two_pi<float>() / 3.0f * time.deltaTime();

				GlobalUbo ubo{};
				ubo.projection = camera.getProjection();
				ubo.view = camera.getView();
				ubo.lightPosition = { cos(currRot) * 1.3f, -1.0f, sin(currRot) * 1.3f };
				uniformBuffers[frameInfo.frameIndex]->writeToBuffer(&ubo);
				uniformBuffers[frameInfo.frameIndex]->flush();

				// Render
				renderer_.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderObjects(frameInfo, objects_);
				pointLightSystem.renderObjects(frameInfo);
				renderer_.endSwapChainRenderPass(commandBuffer);
				renderer_.endFrame();
			}
		}

		vkDeviceWaitIdle(device_.device());
	}


	void Application::loadObjects()
	{
		{
			std::shared_ptr<PhmModel> model = PhmModel::createModelFromFile(device_, "models/smooth_vase.obj");

			PhmObject object;
			object.model = model;
			object.transform.translation = { -0.8f, 0.0f, 0.0f };
			object.transform.scale = glm::vec3(3);
			//object.transform.rotation.x = 0.1f * glm::two_pi<float>();

			objects_.push_back(std::move(object));
		}
		{
			std::shared_ptr<PhmModel> model = PhmModel::createModelFromFile(device_, "models/flat_vase.obj");

			PhmObject object;
			object.model = model;
			object.transform.translation = { 0.8f, 0.0f, 0.0f };
			object.transform.scale = glm::vec3(3);
			//object.transform.rotation.x = 0.1f * glm::two_pi<float>();

			objects_.push_back(std::move(object));
		}
		{
			std::shared_ptr<PhmModel> model = PhmModel::createModelFromFile(device_, "models/quad.obj");

			PhmObject object;
			object.model = model;
			object.transform.translation = { 0.0f, 0.0f, 0.0f };
			object.transform.scale = glm::vec3{ 3.0f, 1.0f, 3.0f };
			//object.transform.rotation.x = 0.1f * glm::two_pi<float>();

			objects_.push_back(std::move(object));
		}
	}
}


