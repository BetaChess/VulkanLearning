#include "phm_app.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <iostream>

std::vector<phm::PhmModel::Vertex> sierpinskiTriangle(std::array<phm::PhmModel::Vertex, 3> initialTriangle, size_t depth)
{
	if (depth == 0)
		return { initialTriangle[0], initialTriangle[1], initialTriangle[2] };

	auto triangles1 = sierpinskiTriangle(
		{
			initialTriangle[0],
			{
				{ // position
					initialTriangle[0].pos.x + (initialTriangle[1].pos.x - initialTriangle[0].pos.x) * 0.5f,
					initialTriangle[0].pos.y + (initialTriangle[1].pos.y - initialTriangle[0].pos.y) * 0.5f
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
					initialTriangle[0].pos.y + (initialTriangle[2].pos.y - initialTriangle[0].pos.y) * 0.5f
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
					initialTriangle[1].pos.y + (initialTriangle[0].pos.y - initialTriangle[1].pos.y) * 0.5f
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
					initialTriangle[1].pos.y + (initialTriangle[2].pos.y - initialTriangle[1].pos.y) * 0.5f
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
					initialTriangle[2].pos.y + (initialTriangle[0].pos.y - initialTriangle[2].pos.y) * 0.5f
				},
				{ // Color
					initialTriangle[2].col.r + (initialTriangle[0].col.r - initialTriangle[2].col.r) * 0.5f,
					initialTriangle[2].col.g + (initialTriangle[0].col.g - initialTriangle[2].col.g) * 0.5f,
					initialTriangle[2].col.b + (initialTriangle[0].col.b - initialTriangle[2].col.b) * 0.5f
				}
			},
			{
				{ // Position
					initialTriangle[2].pos.x + (initialTriangle[1].pos.x - initialTriangle[2].pos.x) * 0.5f,
					initialTriangle[2].pos.y + (initialTriangle[1].pos.y - initialTriangle[2].pos.y) * 0.5f
				},
				{ // Color
					initialTriangle[2].col.r + (initialTriangle[1].col.r - initialTriangle[2].col.r) * 0.5f,
					initialTriangle[2].col.g + (initialTriangle[1].col.g - initialTriangle[2].col.g) * 0.5f,
					initialTriangle[2].col.b + (initialTriangle[1].col.b - initialTriangle[2].col.b) * 0.5f
				}
			}
		}, depth - 1);

	std::vector<phm::PhmModel::Vertex> retVec;
	retVec.insert(retVec.end(), triangles1.begin(), triangles1.end());
	retVec.insert(retVec.end(), triangles2.begin(), triangles2.end());
	retVec.insert(retVec.end(), triangles3.begin(), triangles3.end());

	return retVec;
}

namespace phm
{
	struct SimplePushConstantData
	{
		glm::mat2 transform{ 1.0f };
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};

	Application::Application()
	{
		loadObjects();
		createPipelineLayout();
		recreateSwapchain(); // Calls createPipeline()
		createCommandBuffers();
	}

	Application::~Application()
	{
		vkDestroyPipelineLayout(device_.device(), pipelineLayout_, nullptr);
	}

	void Application::run()
	{
		while (!window_.shouldClose())
		{
			glfwPollEvents();
			drawFrame();
		}

		vkDeviceWaitIdle(device_.device());
	}

	// Self documenting
	void Application::createPipelineLayout()
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};

		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.setLayoutCount = 0;
		pipelineLayoutCreateInfo.pSetLayouts = nullptr;
		pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
		pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(device_.device(), &pipelineLayoutCreateInfo, nullptr, &pipelineLayout_) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create pipeline layout. ");
		}
	}

	void Application::loadObjects()
	{
		/*std::vector<PhmModel::Vertex> vertices
		{
			{{0.0f, -0.5f}},
			{{0.5f, 0.5f}},
			{{-0.5f, 0.5f}}
		};*/
		std::vector<PhmModel::Vertex> vertices = sierpinskiTriangle(
			{ { 
				{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
				{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
				{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
			} }, 6);

		auto model = std::make_shared<PhmModel>(device_, vertices);

		PhmObject obj{};
		obj.model = model;
		obj.color = { 0.1f, 0.8f,0.1f };
		obj.transform.translation.x = 0.2f;
		obj.transform.scale = { 1.5f, 0.75f };
		obj.transform.setRotation(0.05f * glm::two_pi<float>());

		objects_.push_back(std::move(obj));
	}

	void Application::createPipeline()
	{
		assert(
			swapchain_ != nullptr &&
			"Cannot create pipline before swap chain"
		);
		assert(
			pipelineLayout_ != nullptr &&
			"Cannot create pipeline before the pipeline layout"
		);

		phm::PipelineConfigInfo pipelineConfig{};
		PhmPipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = swapchain_->getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout_;

		pipeline_ = std::make_unique<PhmPipeline>(
			device_,
			"shaders/simple_shader.vert.spv",
			"shaders/simple_shader.frag.spv",
			pipelineConfig
			);

	}

	void Application::recreateSwapchain()
	{
		auto extent = window_.getExtent();

		// This pauses the application if it is minimised.
		while (extent.height == 0 || extent.width == 0)
		{
			extent = window_.getExtent();
			glfwWaitEvents();
		}

		// Wait for the swapchain to be unused.
		vkDeviceWaitIdle(device_.device());

		if (swapchain_ == nullptr)
		{
			swapchain_ = std::make_unique<PhmSwapchain>(device_, extent);
		}
		else
		{
			swapchain_ = std::make_unique<PhmSwapchain>(device_, extent, std::move(swapchain_));
			if (swapchain_->imageCount() != commandBuffers_.size())
			{
				freeCommandBuffers();
				createCommandBuffers();
			}
		}

		createPipeline();
	}

	void Application::createCommandBuffers()
	{
		commandBuffers_.resize(swapchain_->imageCount());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = device_.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers_.size());

		if (vkAllocateCommandBuffers(device_.device(), &allocInfo, commandBuffers_.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate command buffers");
		}
	}

	void Application::freeCommandBuffers()
	{
		vkFreeCommandBuffers(device_.device(), device_.getCommandPool(), static_cast<uint32_t>(commandBuffers_.size()), commandBuffers_.data());
		commandBuffers_.clear();
	}

	void Application::recordCommandBuffer(size_t imageIndex)
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffers_[imageIndex], &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to begin recording command buffer");
		}

		VkRenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = swapchain_->getRenderPass();
		renderPassBeginInfo.framebuffer = swapchain_->getFrameBuffer(imageIndex);

		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.renderArea.extent = swapchain_->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassBeginInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers_[imageIndex], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(swapchain_->getSwapChainExtent().width);
		viewport.height = static_cast<float>(swapchain_->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffers_[imageIndex], 0, 1, &viewport);

		VkRect2D scissor;
		scissor.offset = { 0,0 };
		scissor.extent = swapchain_->getSwapChainExtent();
		vkCmdSetScissor(commandBuffers_[imageIndex], 0, 1, &scissor);


		renderGameObjects(commandBuffers_[imageIndex]);


		vkCmdEndRenderPass(commandBuffers_[imageIndex]);
		if (vkEndCommandBuffer(commandBuffers_[imageIndex]) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to end recording command buffer");
		}
	}

	void Application::renderGameObjects(VkCommandBuffer commandBuffer)
	{
		pipeline_->bind(commandBuffer);

		for (auto& obj : objects_)
		{
			SimplePushConstantData push{};
			push.offset = obj.transform.translation;
			push.color = obj.color;
			push.transform = obj.transform.mat2();

			vkCmdPushConstants(
				commandBuffer,
				pipelineLayout_,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(SimplePushConstantData),
				&push);
			obj.model->bind(commandBuffer);
			obj.model->draw(commandBuffer);

		}
	}

	void Application::drawFrame()
	{
		uint32_t imageIndex;
		VkResult result = swapchain_->acquireNextImage(&imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapchain();
			return;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("Failed to acquire swap chain image");
		}

		recordCommandBuffer(imageIndex);
		result = swapchain_->submitCommandBuffers(&commandBuffers_[imageIndex], &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window_.wasWindowResized())
		{
			window_.resetWindowResizedFlag();
			recreateSwapchain();
			return;
		}

		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to present swap chain image");
		}
	}
}


