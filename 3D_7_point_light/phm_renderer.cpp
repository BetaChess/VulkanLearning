#include "pch.h"

#include "phm_renderer.h"

#include <stdexcept>
#include <array>
#include <iostream>


namespace phm
{

	PhmRenderer::PhmRenderer(PhmWindow& window, PhmDevice& device)
		: window_(window), device_(device)
	{
		recreateSwapchain(); // Calls createPipeline()
		createCommandBuffers();
	}

	PhmRenderer::~PhmRenderer()
	{
		freeCommandBuffers();
	}

	void PhmRenderer::recreateSwapchain()
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
			std::shared_ptr<PhmSwapchain> oldSwapChain = std::move(swapchain_);
			swapchain_ = std::make_unique<PhmSwapchain>(device_, extent, oldSwapChain);

			if (!oldSwapChain->compareSwapChainFormats(*swapchain_.get()))
			{
				throw std::runtime_error("Swap chain image (or depth) format has changed");
			}
		}
	}

	void PhmRenderer::createCommandBuffers()
	{
		commandBuffers_.resize(PhmSwapchain::MAX_FRAMES_IN_FLIGHT);

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

	void PhmRenderer::freeCommandBuffers()
	{
		vkFreeCommandBuffers(device_.device(), device_.getCommandPool(), static_cast<uint32_t>(commandBuffers_.size()), commandBuffers_.data());
		commandBuffers_.clear();
	}

	VkCommandBuffer PhmRenderer::beginFrame()
	{
		assert(!isFrameStarted_ && "Cannot call beginFrame while already in progress");


		VkResult result = swapchain_->acquireNextImage(&currentImageIndex_);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapchain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("Failed to acquire swap chain image");
		}

		isFrameStarted_ = true;

		// Get the current command buffer
		auto commandBuffer = getCurrentCommandBuffer();

		// "begin" the command buffer
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to begin recording command buffer");
		}

		return commandBuffer;
	}

	void PhmRenderer::endFrame()
	{
		assert(isFrameStarted_ && "Can't end frame while frame is not in progess");
		auto commandBuffer = getCurrentCommandBuffer();

		// "End" the command buffer
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to end recording command buffer");
		}

		auto result = swapchain_->submitCommandBuffers(&commandBuffer, &currentImageIndex_);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window_.wasWindowResized())
		{
			window_.resetWindowResizedFlag();
			recreateSwapchain();
		}
		else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to present swap chain image");
		}

		isFrameStarted_ = false;
		currentFrameIndex_ = (currentFrameIndex_ + 1) % PhmSwapchain::MAX_FRAMES_IN_FLIGHT;
	}

	void PhmRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted_ && "Can't begin swap chain render pass while frame is not in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "Can't start a render pass with a command buffer from a different frame");

		VkRenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = swapchain_->getRenderPass();
		renderPassBeginInfo.framebuffer = swapchain_->getFrameBuffer(currentImageIndex_);

		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.renderArea.extent = swapchain_->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassBeginInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(swapchain_->getSwapChainExtent().width);
		viewport.height = static_cast<float>(swapchain_->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor;
		scissor.offset = { 0,0 };
		scissor.extent = swapchain_->getSwapChainExtent();
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void PhmRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted_ && "Can't end swap chain render pass while frame is not in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "Can't end a render pass with a command buffer from a different frame");
		vkCmdEndRenderPass(commandBuffer);
	}
}
