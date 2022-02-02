#include "pch.h"

#include "phm_app.h"

#include "phm_keyboardController.h"
#include "phm_camera.h"
#include "simple_render_system.h"
#include "point_light_system.h"
#include "frustum_render_system.h"
#include "time.h"


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <iostream>


glm::mat4 lerp(const glm::mat4& mata, const glm::mat4& matb, float lerp)
{
	glm::mat4 retMat;

	for (size_t i = 0; i < 4; i++)
		for (size_t j = 0; j < 4; j++)
			retMat[i][j] = mata[i][j] + (matb[i][j] - mata[i][j]) * lerp;

	return retMat;
}

namespace phm
{
	PhmObject getViewFrustum(PhmDevice& device, float fovy, float aspect, float near, float far)
	{
		PhmObject retObj;

		float neary = glm::sin(fovy) * near;
		float nearx = aspect * neary;
		float fary = glm::sin(fovy) * far;
		float farx = aspect * fary;

		glm::vec4 color{ 105.0f / 255.0f, 14.0f / 255.0f, 232.0f / 255.0f, 0.05f };

		PhmModel::Builder builder;
		// Normals and UV, not used
		builder.vertices.push_back(
			{
				{nearx, neary, near}, // position
				color // color
			}
		);
		builder.vertices.push_back(
			{
				{-nearx, neary, near}, // position
				color // color
			}
		);
		builder.vertices.push_back(
			{
				{nearx, -neary, near}, // position
				color // color
			}
		);
		builder.vertices.push_back(
			{
				{-nearx, -neary, near}, // position
				color // color
			}
		);


		builder.vertices.push_back(
			{
				{farx, fary, far}, // position
				color // color
			}
		);
		builder.vertices.push_back(
			{
				{-farx, fary, far}, // position
				color // color
			}
		);
		builder.vertices.push_back(
			{
				{farx, -fary, far}, // position
				color // color
			}
		);
		builder.vertices.push_back(
			{
				{-farx, -fary, far}, // position
				color // color
			}
		);
		// Near plane face
		builder.indices.push_back(1);
		builder.indices.push_back(0);
		builder.indices.push_back(3);

		builder.indices.push_back(0);
		builder.indices.push_back(2);
		builder.indices.push_back(3);

		// Far plane face
		builder.indices.push_back(7);
		builder.indices.push_back(4);
		builder.indices.push_back(5);
		
		builder.indices.push_back(6);
		builder.indices.push_back(7);
		builder.indices.push_back(4);

		// Left plane face
		builder.indices.push_back(1); // +y left near
		builder.indices.push_back(3); // -y left near
		builder.indices.push_back(7); // -y left far

		builder.indices.push_back(1); // +y left near
		builder.indices.push_back(5); // +y left far
		builder.indices.push_back(7); // -y left far

		// Right plane face
		builder.indices.push_back(0); // +y right near
		builder.indices.push_back(2); // -y right near
		builder.indices.push_back(6); // -y right far

		builder.indices.push_back(0); // +y right near
		builder.indices.push_back(4); // +y right far
		builder.indices.push_back(6); // -y right far

		// +y plane face
		builder.indices.push_back(0); // +y right near
		builder.indices.push_back(1); // +y left near
		builder.indices.push_back(5); // +y left far

		builder.indices.push_back(4); // +y right far
		builder.indices.push_back(5); // +y left far
		builder.indices.push_back(0); // +y right near

		// -y plane face
		builder.indices.push_back(2); // -y right near
		builder.indices.push_back(3); // -y left near
		builder.indices.push_back(7); // -y left far

		builder.indices.push_back(6); // -y right far
		builder.indices.push_back(7); // -y left far
		builder.indices.push_back(2); // -y right near


		retObj.model = std::make_unique<PhmModel>(device, builder);

		return retObj;
	}

	struct GlobalUbo
	{
		glm::mat4 projection{ 1.0f };
		glm::mat4 view{ 1.0f };
		glm::mat4 projection2{ 1.0f };
		glm::mat4 view2{ 1.0f };
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
		FrustumRenderSystem frustumRenderSystem{ device_, renderer_.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };

		
		// Cameras
		PhmCamera camera{};
		PhmObject cameraViewerObject;
		cameraViewerObject.transform.translation.z = -2.3f;
		cameraViewerObject.transform.translation.y = -0.5f;

		bool useBothProjections = false;
		bool usingProjectionViewerCamera = false;
		PhmCamera projectionViewerCamera{};
		PhmObject projectionViewerObject;
		projectionViewerObject.transform.translation.z = -3.3f;
		projectionViewerObject.transform.translation.y = -0.8f;
		projectionViewerObject.transform.rotation.y = -0.1f;

		// non owning pointers to the currently used camera.
		PhmCamera* currentCamera = &camera;
		PhmObject* currentCameraObject = &cameraViewerObject; 

		// Input
		PhmKeyboardController cameraController{};

		// Time management
		Time time;

		float currLerp = 0.0f;
		float timeUntilMaxLerp = 1.5f;

		PhmObject frustum;

		float currRot = 0;
		while (!window_.shouldClose())
		{
			glfwPollEvents();

			time.updateTime();

			currLerp += time.deltaTime();

			// Check if the view has to be changed in any way
			if (cameraController.shouldChangeCamera(window_.getGLFWWindow()))
			{
				usingProjectionViewerCamera = !usingProjectionViewerCamera;

				if (usingProjectionViewerCamera)
				{
					currentCamera = &projectionViewerCamera;
					currentCameraObject = &projectionViewerObject;
				}
				else
				{
					currentCamera = &camera;
					currentCameraObject = &cameraViewerObject;
				}
			}

			if (cameraController.shouldChangeProjectionView(window_.getGLFWWindow()))
			{
				useBothProjections = !useBothProjections;
				if (useBothProjections)
					currLerp = 0;
			}

			// Get movement input
			cameraController.moveInPlaneXZ(window_.getGLFWWindow(), time.deltaTime(), *currentCameraObject);
			currentCamera->setViewYXZ(currentCameraObject->transform.translation, currentCameraObject->transform.rotation);

			// Calculate the perspective
			float aspect = renderer_.getAspectRatio();
			camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 100.0f);
			projectionViewerCamera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 200.0f);
			
			// BeginFrame returns a nullptr if the swapchain needs to be recreated. 
			// This skips the frame draw call, if that's the case.
			auto commandBuffer = renderer_.beginFrame();
			if (commandBuffer != nullptr)
			{
				const FrameInfo frameInfo{
					renderer_.getFrameIndex(),
					time.deltaTime(),
					commandBuffer,
					*currentCamera,
					globalDescriptorSets[renderer_.getFrameIndex()]
				};

				// Update
				currRot += glm::two_pi<float>() / 3.0f * time.deltaTime();

				// Update the UBO
				GlobalUbo ubo{};
				ubo.projection = currentCamera->getProjection();
				ubo.view = currentCamera->getView();
				if (useBothProjections && usingProjectionViewerCamera)
				{ 
					ubo.projection2 = lerp(glm::mat4{ 1.0f }, camera.getProjection(), std::min(currLerp / timeUntilMaxLerp, 1.0f));
					ubo.view2 = lerp(glm::mat4{ 1.0f }, camera.getView(), std::min(currLerp / timeUntilMaxLerp, 1.0f));
				}
				ubo.lightPosition = { cos(currRot) * 1.3f, -1.0f, sin(currRot) * 1.3f };
				// Flush the UBO to the GPU
				uniformBuffers[frameInfo.frameIndex]->writeToBuffer(&ubo);
				uniformBuffers[frameInfo.frameIndex]->flush();

				// Render
				renderer_.beginSwapChainRenderPass(commandBuffer);

				simpleRenderSystem.renderObjects(frameInfo, objects_);
				pointLightSystem.renderObjects(frameInfo);
				// Render frustum
				if (usingProjectionViewerCamera)
				{
					frustum = getViewFrustum(device_, glm::radians(50.0f), aspect, 0.1f, 100.0f);
					frustum.transform = cameraViewerObject.transform;
					frustumRenderSystem.renderObjects(frameInfo, { frustum });
				}
				
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


