#include "pch.h"

#include "phm_swapchain.h"

#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <set>
#include <stdexcept>

namespace phm
{
	PhmSwapchain::PhmSwapchain(PhmDevice& deviceRef, VkExtent2D windowExtent)
		: device_(deviceRef), windowExtent_(windowExtent)
	{
		init();
	}

	PhmSwapchain::PhmSwapchain(PhmDevice& deviceRef, VkExtent2D windowExtent, std::shared_ptr<PhmSwapchain> previous)
		: device_(deviceRef), windowExtent_(windowExtent), oldSwapChain_(previous)
	{
		init();

		oldSwapChain_ = nullptr;
	}

	void PhmSwapchain::init()
	{
		createSwapChain();
		createImageViews();
		createRenderPass();
		createDepthResources();
		createFramebuffers();
		createSyncObjects();
	}

	PhmSwapchain::~PhmSwapchain()
	{
		for (auto imageView : swapChainImageViews_)
		{
			vkDestroyImageView(device_.device(), imageView, nullptr);
		}
		swapChainImageViews_.clear();

		if (swapChain_ != nullptr)
		{
			vkDestroySwapchainKHR(device_.device(), swapChain_, nullptr);
			swapChain_ = nullptr;
		}

		for (int i = 0; i < depthImages_.size(); i++)
		{
			vkDestroyImageView(device_.device(), depthImageViews_[i], nullptr);
			vkDestroyImage(device_.device(), depthImages_[i], nullptr);
			vkFreeMemory(device_.device(), depthImageMemories_[i], nullptr);
		}

		for (auto framebuffer : swapChainFramebuffers_)
		{
			vkDestroyFramebuffer(device_.device(), framebuffer, nullptr);
		}

		vkDestroyRenderPass(device_.device(), renderPass_, nullptr);

		// cleanup synchronization objects
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vkDestroySemaphore(device_.device(), renderFinishedSemaphores_[i], nullptr);
			vkDestroySemaphore(device_.device(), imageAvailableSemaphores_[i], nullptr);
			vkDestroyFence(device_.device(), inFlightFences_[i], nullptr);
		}
	}

	/// <summary>
	/// Aquires the index of the next image.
	/// </summary>
	/// <param name="imageIndex">: The memory location the index should be written to. </param>
	/// <returns>The VkResult of the operation. </returns>
	VkResult PhmSwapchain::acquireNextImage(uint32_t* imageIndex)
	{
		// The next image may already be in fligh, so we wait for the fence to signal that the image is ready.
		vkWaitForFences(
			device_.device(),
			1,											// We're only waiting for a single fence
			&inFlightFences_[currentFrame_],			// We want to wait on the fence associated with the current frame
			VK_TRUE,									// This condition must be satisfied for the wait to end. In this case it is true, 
														//		in which case at least one (there's only one in this case) of the fences 
														//		must be signaled before the wait is dismissed.
			std::numeric_limits<uint64_t>::max());		// We don't want this to time out, so we set the timeout to the limit of 64 bits.

		// We ask vulkan to give us the index of the next image and query the result
		VkResult result = vkAcquireNextImageKHR(
			device_.device(),
			swapChain_,
			std::numeric_limits<uint64_t>::max(),		// Once again, we don't want it to time out.
			imageAvailableSemaphores_[currentFrame_],	// must be a not signaled semaphore
			VK_NULL_HANDLE,
			imageIndex);

		// We return the result
		return result;
	}

	/// <summary>
	/// Submits command buffers to render an image.
	/// </summary>
	/// <param name="buffers">: The buffer(s) to be submited (only a single one for now). </param>
	/// <param name="imageIndex">: The index of the image to render. </param>
	/// <returns>The result of the vkQueuePresentKHR operation. </returns>
	VkResult PhmSwapchain::submitCommandBuffers(
		const VkCommandBuffer* buffers, uint32_t* imageIndex)
	{
		// First we check that the fence of the current image is not signaled. If it's not, we will have to wait for it.
		if (imagesInFlight_[*imageIndex] != VK_NULL_HANDLE)
		{
			vkWaitForFences(device_.device(), 1, &imagesInFlight_[*imageIndex], VK_TRUE, UINT64_MAX);
		}
		// Set the fence for the image index to the in flight fence of the current frame.
		imagesInFlight_[*imageIndex] = inFlightFences_[currentFrame_];

		// Create the submit info for the command buffer
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		// Make an array of the semaphore(s) for synchronization. In this case only one. The semaphore of the current frame.
		VkSemaphore waitSemaphores[] = { imageAvailableSemaphores_[currentFrame_] };
		// Set the flags for which stages should be waited for. 
		// TODO: check if these are the correct flags.
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_VERTEX_SHADER_BIT };
		submitInfo.waitSemaphoreCount = 1;
		// Provide the semaphore(s) and wait stage mask.
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		// Specify the number of command buffers. (only one for now)
		submitInfo.commandBufferCount = 1;
		// Specify the pointer the buffer(s).
		submitInfo.pCommandBuffers = buffers;

		// Make an array of the singal semaphores for when the rendering has finished.
		VkSemaphore signalSemaphores[] = { renderFinishedSemaphores_[currentFrame_] };
		// Provide the semaphore count and the pointer to the array.
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		// Reset the fence.
		vkResetFences(device_.device(), 1, &inFlightFences_[currentFrame_]);

		// Now submit the command buffer(s) and query the result.
		if (vkQueueSubmit(device_.graphicsQueue(), 1, &submitInfo, inFlightFences_[currentFrame_]) !=
			VK_SUCCESS)
		{
			throw std::runtime_error("failed to submit draw command buffer!");
		}

		// Create the present info (the info used to present the image to the screen)
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		// Provide the signal semaphore(s) for the frame in question (the one created earlier).
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		// Provide the swapchain(s) for image presentation
		VkSwapchainKHR swapChains[] = { swapChain_ };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;

		// Provide the index of the image.
		presentInfo.pImageIndices = imageIndex;

		// Put the presentation request in the queue (it will be displayed when the frame is ready).
		auto result = vkQueuePresentKHR(device_.presentQueue(), &presentInfo);

		// Change the current frame of the swapchain.
		currentFrame_ = (currentFrame_ + 1) % MAX_FRAMES_IN_FLIGHT;

		// Return the result of the presentation request.
		return result;
	}

	void PhmSwapchain::createSwapChain()
	{ 
		// Queries the device for swapchain support details.
		SwapChainSupportDetails swapChainSupport = device_.getSwapChainSupport();

		// Query the swapchain method for choosing the surface format.
		VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
		// Query the swapchain method for choosing the present mode.
		VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
		// Query the swapchain method for choosing the swap chain extent.
		VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

		// Choose an image count.
		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
		if (swapChainSupport.capabilities.maxImageCount > 0 &&
			imageCount > swapChainSupport.capabilities.maxImageCount)
		{
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		// create swapchain create info
		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		// Specify the surface
		createInfo.surface = device_.surface();

		// Specify the imagecount
		createInfo.minImageCount = imageCount;
		// Sepcify the image format, colorspace, extent and arraylayers (Constant).
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		// Specify that the swapchain should be a color attachment swapchain (as opposed to a depth stencil for example)
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		// Get the indeces of the physical queue families using the method of device_.
		QueueFamilyIndices indices = device_.findPhysicalQueueFamilies();
		// Specify the index of each index in a C style array.
		uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		if (indices.graphicsFamily != indices.presentFamily)
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;      // Optional
			createInfo.pQueueFamilyIndices = nullptr;  // Optional
		}

		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;

		createInfo.oldSwapchain = oldSwapChain_ == nullptr ? VK_NULL_HANDLE : oldSwapChain_->swapChain_;

		if (vkCreateSwapchainKHR(device_.device(), &createInfo, nullptr, &swapChain_) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create swap chain!");
		}

		// we only specified a minimum number of images in the swap chain, so the implementation is
		// allowed to create a swapchain with more. That's why we'll first query the final number of
		// images with vkGetSwapchainImagesKHR, then resize the container and finally call it again to
		// retrieve the handles.
		vkGetSwapchainImagesKHR(device_.device(), swapChain_, &imageCount, nullptr);
		swapChainImages_.resize(imageCount);
		vkGetSwapchainImagesKHR(device_.device(), swapChain_, &imageCount, swapChainImages_.data());

		swapChainImageFormat_ = surfaceFormat.format;
		swapChainExtent_ = extent;
	}

	void PhmSwapchain::createImageViews()
	{
		swapChainImageViews_.resize(swapChainImages_.size());

		// Iterate over each image view and initialise it. 
		for (size_t i = 0; i < swapChainImages_.size(); i++)
		{
			VkImageViewCreateInfo viewInfo{};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = swapChainImages_[i];
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = swapChainImageFormat_;
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(device_.device(), &viewInfo, nullptr, &swapChainImageViews_[i]) !=
				VK_SUCCESS)
			{
				throw std::runtime_error("failed to create texture image view!");
			}
		}
	}

	void PhmSwapchain::createRenderPass()
	{
		VkAttachmentDescription depthAttachment{};
		depthAttachment.format = findDepthFormat();
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef{};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = getSwapChainImageFormat();
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;

		VkSubpassDependency dependency = {};

		dependency.dstSubpass = 0;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.srcAccessMask = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

		std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(device_.device(), &renderPassInfo, nullptr, &renderPass_) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create render pass!");
		}
	}

	void PhmSwapchain::createFramebuffers()
	{
		swapChainFramebuffers_.resize(imageCount());

		for (size_t i = 0; i < imageCount(); i++)
		{
			std::array<VkImageView, 2> attachments = { swapChainImageViews_[i], depthImageViews_[i] };

			VkExtent2D swapChainExtent_ = getSwapChainExtent();
			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderPass_;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = swapChainExtent_.width;
			framebufferInfo.height = swapChainExtent_.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(
				device_.device(),
				&framebufferInfo,
				nullptr,
				&swapChainFramebuffers_[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create framebuffer!");
			}
		}
	}

	void PhmSwapchain::createDepthResources()
	{
		VkFormat depthFormat = findDepthFormat();
		swapChainDepthFormat_ = depthFormat;
		VkExtent2D swapChainExtent_ = getSwapChainExtent();

		depthImages_.resize(imageCount());
		depthImageMemories_.resize(imageCount());
		depthImageViews_.resize(imageCount());

		for (int i = 0; i < depthImages_.size(); i++)
		{
			VkImageCreateInfo imageInfo{};
			imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.extent.width = swapChainExtent_.width;
			imageInfo.extent.height = swapChainExtent_.height;
			imageInfo.extent.depth = 1;
			imageInfo.mipLevels = 1;
			imageInfo.arrayLayers = 1;
			imageInfo.format = depthFormat;
			imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageInfo.flags = 0;

			device_.createImageWithInfo(
				imageInfo,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				depthImages_[i],
				depthImageMemories_[i]);

			VkImageViewCreateInfo viewInfo{};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = depthImages_[i];
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = depthFormat;
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(device_.device(), &viewInfo, nullptr, &depthImageViews_[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create texture image view!");
			}
		}
	}

	void PhmSwapchain::createSyncObjects()
	{
		imageAvailableSemaphores_.resize(MAX_FRAMES_IN_FLIGHT);
		renderFinishedSemaphores_.resize(MAX_FRAMES_IN_FLIGHT);
		inFlightFences_.resize(MAX_FRAMES_IN_FLIGHT);
		imagesInFlight_.resize(imageCount(), VK_NULL_HANDLE);

		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo = {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			if (vkCreateSemaphore(device_.device(), &semaphoreInfo, nullptr, &imageAvailableSemaphores_[i]) !=
				VK_SUCCESS ||
				vkCreateSemaphore(device_.device(), &semaphoreInfo, nullptr, &renderFinishedSemaphores_[i]) !=
				VK_SUCCESS ||
				vkCreateFence(device_.device(), &fenceInfo, nullptr, &inFlightFences_[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create synchronization objects for a frame!");
			}
		}
	}

	VkSurfaceFormatKHR PhmSwapchain::chooseSwapSurfaceFormat(
		const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		for (const auto& availableFormat : availableFormats)
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
				availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return availableFormat;
			}
		}

		return availableFormats[0];
	}

	VkPresentModeKHR PhmSwapchain::chooseSwapPresentMode(
		const std::vector<VkPresentModeKHR>& availablePresentModes)
	{
		for (const auto& availablePresentMode : availablePresentModes)
		{
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				std::cout << "Present mode: Mailbox" << std::endl;
				return availablePresentMode;
			}
		}

		std::cout << "Present mode: V-Sync" << std::endl;
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D PhmSwapchain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			return capabilities.currentExtent;
		}
		else
		{
			VkExtent2D actualExtent = windowExtent_;
			actualExtent.width = std::max(
				capabilities.minImageExtent.width,
				std::min(capabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max(
				capabilities.minImageExtent.height,
				std::min(capabilities.maxImageExtent.height, actualExtent.height));

			return actualExtent;
		}
	}

	VkFormat PhmSwapchain::findDepthFormat()
	{
		return device_.findSupportedFormat(
			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	}

}  // namespace phm
