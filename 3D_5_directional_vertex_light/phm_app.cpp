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
	Application::Application()
	{
		loadObjects();
	}

	Application::~Application()
	{
	}

	void Application::run()
	{
		SimpleRenderSystem simpleRenderSystem{ device_, renderer_.getSwapChainRenderPass() };
		PhmCamera camera{};
		PhmObject viewerObject;
		PhmKeyboardController cameraController{};
		//camera.setViewDirection(glm::vec3(0.0f), glm::vec3(0.5f, 0.0f, 1.0f));

		Time time;

		while (!window_.shouldClose())
		{
			glfwPollEvents();

			time.updateTime();

			cameraController.moveInPlaneXZ(window_.getGLFWWindow(), time.deltaTime(), viewerObject);
			camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

			float aspect = renderer_.getAspectRatio();
			camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 10.0f);
			
			// BeginFrame returns a nullptr if the swapchain needs to be recreated. 
			// This skips the frame draw call, if that's the case.
			auto commandBuffer = renderer_.beginFrame();
			if (commandBuffer != nullptr)
			{
				renderer_.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderObjects(commandBuffer, objects_, camera);
				renderer_.endSwapChainRenderPass(commandBuffer);
				renderer_.endFrame();
			}
		}

		vkDeviceWaitIdle(device_.device());
	}


	void Application::loadObjects()
	{
		std::shared_ptr<PhmModel> model = PhmModel::createModelFromFile(device_, "models/smooth_vase.obj");

		PhmObject object;
		object.model = model;
		object.transform.translation = { 0.0f, 0.0f, 1.5f };
		object.transform.scale = glm::vec3(3);
		//object.transform.rotation.x = 0.1f * glm::two_pi<float>();
		
		objects_.push_back(std::move(object));
	}
}


