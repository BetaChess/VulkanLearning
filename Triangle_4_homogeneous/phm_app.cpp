#include "phm_app.h"

#include "simple_render_system.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <iostream>

std::vector<phm::Model::Vertex> sierpinskiTriangle(std::array<phm::Model::Vertex, 3> initialTriangle, size_t depth)
{
	if (depth == 0)
		return { initialTriangle[0], initialTriangle[1], initialTriangle[2] };

	auto triangles1 = sierpinskiTriangle(
		{
			initialTriangle[0],
			{
				{ // position
					initialTriangle[0].pos.x + (initialTriangle[1].pos.x - initialTriangle[0].pos.x) * 0.5f,
					initialTriangle[0].pos.y + (initialTriangle[1].pos.y - initialTriangle[0].pos.y) * 0.5f,
					1.0f
				},
				{ // Color
					initialTriangle[0].col.r + (initialTriangle[1].col.r - initialTriangle[0].col.r) * 0.5f,
					initialTriangle[0].col.g + (initialTriangle[1].col.g - initialTriangle[0].col.g) * 0.5f,
					initialTriangle[0].col.b + (initialTriangle[1].col.b - initialTriangle[0].col.b) * 0.5f
				}
			},
			{
				{ // Position
					initialTriangle[0].pos.x + (initialTriangle[2].pos.x - initialTriangle[0].pos.x) * 0.5f, 
					initialTriangle[0].pos.y + (initialTriangle[2].pos.y - initialTriangle[0].pos.y) * 0.5f,
					1.0f
				},
				{ // Color
					initialTriangle[0].col.r + (initialTriangle[2].col.r - initialTriangle[0].col.r) * 0.5f,
					initialTriangle[0].col.g + (initialTriangle[2].col.g - initialTriangle[0].col.g) * 0.5f,
					initialTriangle[0].col.b + (initialTriangle[2].col.b - initialTriangle[0].col.b) * 0.5f
				}
			}
		}, depth - 1);
	auto triangles2 = sierpinskiTriangle(
		{
			initialTriangle[1],
			{
				{ // position
					initialTriangle[1].pos.x + (initialTriangle[0].pos.x - initialTriangle[1].pos.x) * 0.5f,
					initialTriangle[1].pos.y + (initialTriangle[0].pos.y - initialTriangle[1].pos.y) * 0.5f,
					1.0f
				},
				{ // Color
					initialTriangle[1].col.r + (initialTriangle[0].col.r - initialTriangle[1].col.r) * 0.5f,
					initialTriangle[1].col.g + (initialTriangle[0].col.g - initialTriangle[1].col.g) * 0.5f,
					initialTriangle[1].col.b + (initialTriangle[0].col.b - initialTriangle[1].col.b) * 0.5f
				}
			},
			{
				{ // Position
					initialTriangle[1].pos.x + (initialTriangle[2].pos.x - initialTriangle[1].pos.x) * 0.5f,
					initialTriangle[1].pos.y + (initialTriangle[2].pos.y - initialTriangle[1].pos.y) * 0.5f,
					1.0f
				},
				{ // Color
					initialTriangle[1].col.r + (initialTriangle[2].col.r - initialTriangle[1].col.r) * 0.5f,
					initialTriangle[1].col.g + (initialTriangle[2].col.g - initialTriangle[1].col.g) * 0.5f,
					initialTriangle[1].col.b + (initialTriangle[2].col.b - initialTriangle[1].col.b) * 0.5f
				}
			}
		}, depth - 1);
	auto triangles3 = sierpinskiTriangle(
		{
			initialTriangle[2],
			{
				{ // position
					initialTriangle[2].pos.x + (initialTriangle[0].pos.x - initialTriangle[2].pos.x) * 0.5f,
					initialTriangle[2].pos.y + (initialTriangle[0].pos.y - initialTriangle[2].pos.y) * 0.5f,
					1.0f
				},
				{ // Color
					initialTriangle[2].col.r + (initialTriangle[0].col.r - initialTriangle[2].col.r) * 0.5f,
					initialTriangle[2].col.g + (initialTriangle[0].col.g - initialTriangle[2].col.g) * 0.5f,
					initialTriangle[2].col.b + (initialTriangle[0].col.b - initialTriangle[2].col.b) * 0.5f,
				}
			},
			{
				{ // Position
					initialTriangle[2].pos.x + (initialTriangle[1].pos.x - initialTriangle[2].pos.x) * 0.5f,
					initialTriangle[2].pos.y + (initialTriangle[1].pos.y - initialTriangle[2].pos.y) * 0.5f,
					1.0f
				},
				{ // Color
					initialTriangle[2].col.r + (initialTriangle[1].col.r - initialTriangle[2].col.r) * 0.5f,
					initialTriangle[2].col.g + (initialTriangle[1].col.g - initialTriangle[2].col.g) * 0.5f,
					initialTriangle[2].col.b + (initialTriangle[1].col.b - initialTriangle[2].col.b) * 0.5f
				}
			}
		}, depth - 1);

	std::vector<phm::Model::Vertex> retVec;
	retVec.insert(retVec.end(), triangles1.begin(), triangles1.end());
	retVec.insert(retVec.end(), triangles2.begin(), triangles2.end());
	retVec.insert(retVec.end(), triangles3.begin(), triangles3.end());

	return retVec;
}

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

		while (!window_.shouldClose())
		{
			glfwPollEvents();

			// BeginFrame returns a nullptr if the swapchain needs to be recreated. 
			// This skips the frame draw call, if that's the case.
			auto commandBuffer = renderer_.beginFrame();
			if (commandBuffer != nullptr)
			{
				renderer_.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderObjects(commandBuffer, objects_);
				renderer_.endSwapChainRenderPass(commandBuffer);
				renderer_.endFrame();
			}
		}

		vkDeviceWaitIdle(device_.device());
	}


	void Application::loadObjects()
	{
		std::vector<Model::Vertex> vertices = sierpinskiTriangle(
			{ { 
				{{0.0f, -0.5f, 1.0f}, {1.0f, 0.0f, 0.0f}},
				{{0.5f, 0.5f, 1.0f}, {0.0f, 1.0f, 0.0f}},
				{{-0.5f, 0.5f, 1.0f}, {0.0f, 0.0f, 1.0f}}
			} }, 6);

		/*std::vector<Model::Vertex> vertices = {
			{{0.0f, -0.5f, 1.0f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f, 1.0f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f, 1.0f}, {0.0f, 0.0f, 1.0f}}
		};*/
		
		auto model = std::make_shared<Model>(device_, vertices);

		Object obj{};
		obj.model = model;
		obj.color = { 0.7f, 0.8f,0.15f };
		obj.transform.translation.x = 0.0f;
		obj.transform.translation.y = 0.3f;
		obj.transform.scale = { 1.0f, 1.0f };
		obj.transform.setRotation(0.05f * glm::two_pi<float>());

		objects_.push_back(std::move(obj));
	}
}


