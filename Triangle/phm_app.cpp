#include "phm_app.h"

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
	Application::Application()
	{
		loadModels();
		createPipelineLayout();
		createPipeline();
		createCommandBuffers();
	}

	Application::~Application()
	{
		vkDestroyPipelineLayout(m_device.device(), m_pipelineLayout, nullptr);
	}

	void Application::run()
	{
		while (!m_window.shouldClose())
		{
			glfwPollEvents();
			drawFrame();
		}

		vkDeviceWaitIdle(m_device.device());
	}

	// Self documenting
	void Application::createPipelineLayout()
	{
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};

		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.setLayoutCount = 0;
		pipelineLayoutCreateInfo.pSetLayouts = nullptr;
		pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
		pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

		if (vkCreatePipelineLayout(m_device.device(), &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create pipeline layout. ");
		}
	}

	void Application::loadModels()
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

		m_model = std::make_unique<PhmModel>(m_device, vertices);
	}

	void Application::createPipeline()
	{
		phm::PipelineConfigInfo pipelineConfig{};
		PhmPipeline::defaultPipelineConfigInfo(pipelineConfig, m_window.getExtent().width, m_window.getExtent().height);
		pipelineConfig.renderPass = m_swapchain.getRenderPass();
		pipelineConfig.pipelineLayout = m_pipelineLayout;

		m_pipeline = std::make_unique<PhmPipeline>(
			m_device,
			"shaders/simple_shader.vert.spv",
			"shaders/simple_shader.frag.spv",
			pipelineConfig
			);

	}

	void Application::createCommandBuffers()
	{
		m_commandBuffers.resize(m_swapchain.imageCount());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = m_device.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size());

		if (vkAllocateCommandBuffers(m_device.device(), &allocInfo, m_commandBuffers.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate command buffers");
		}

		for (size_t i = 0; i < m_commandBuffers.size(); i++)
		{
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			if (vkBeginCommandBuffer(m_commandBuffers[i], &beginInfo) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to begin recording command buffer");
			}

			VkRenderPassBeginInfo renderPassBeginInfo{};
			renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassBeginInfo.renderPass = m_swapchain.getRenderPass();
			renderPassBeginInfo.framebuffer = m_swapchain.getFrameBuffer(i);

			renderPassBeginInfo.renderArea.offset = { 0, 0 };
			renderPassBeginInfo.renderArea.extent = m_swapchain.getSwapChainExtent();

			std::array<VkClearValue, 2> clearValues{};
			clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
			clearValues[1].depthStencil = { 1.0f, 0 };

			renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassBeginInfo.pClearValues = clearValues.data();

			vkCmdBeginRenderPass(m_commandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

			m_pipeline->bind(m_commandBuffers[i]);
			m_model->bind(m_commandBuffers[i]);
			m_model->draw(m_commandBuffers[i]);

			vkCmdEndRenderPass(m_commandBuffers[i]);
			if (vkEndCommandBuffer(m_commandBuffers[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to end recording command buffer");
			}
		}
	}

	void Application::drawFrame()
	{
		uint32_t imageIndex;
		VkResult result = m_swapchain.acquireNextImage(&imageIndex);

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("Failed to acquire swap chain image");
		}

		result = m_swapchain.submitCommandBuffers(&m_commandBuffers[imageIndex], &imageIndex);

		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to present swap chain image");
		}
	}
}


