#ifndef PHM_SWAPCHAIN_H
#define PHM_SWAPCHAIN_H

#include "phm_device.h"

// vulkan headers
#include <vulkan/vulkan.h>

// stdlib
#include <memory>
#include <string>
#include <vector>

namespace phm
{
	class PhmSwapchain
	{
	public:
		static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

		PhmSwapchain(PhmDevice& deviceRef, VkExtent2D windowExtent);
		PhmSwapchain(PhmDevice& deviceRef, VkExtent2D windowExtent, std::shared_ptr<PhmSwapchain> previous);
		~PhmSwapchain();

		// Not copyable or movable
		PhmSwapchain(const PhmSwapchain&) = delete;
		PhmSwapchain& operator=(const PhmSwapchain&) = delete;
		PhmSwapchain(PhmSwapchain&&) = delete;
		PhmSwapchain& operator=(PhmSwapchain&&) = delete;


		VkFramebuffer getFrameBuffer(size_t index) { return swapChainFramebuffers_[index]; }
		VkRenderPass getRenderPass() { return renderPass_; }
		VkImageView getImageView(size_t index) { return swapChainImageViews_[index]; }
		size_t imageCount() { return swapChainImages_.size(); }
		VkFormat getSwapChainImageFormat() { return swapChainImageFormat_; }
		VkExtent2D getSwapChainExtent() { return swapChainExtent_; }
		uint32_t width() { return swapChainExtent_.width; }
		uint32_t height() { return swapChainExtent_.height; }

		float extentAspectRatio()
		{
			return static_cast<float>(swapChainExtent_.width) / static_cast<float>(swapChainExtent_.height);
		}
		VkFormat findDepthFormat();

		VkResult acquireNextImage(uint32_t* imageIndex);
		VkResult submitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex);

		inline bool compareSwapChainFormats(const PhmSwapchain& swapChain) const { 
			return swapChainDepthFormat_ == swapChain.swapChainDepthFormat_ &&
				   swapChainImageFormat_ == swapChain.swapChainImageFormat_;
		}

	private:
		VkFormat swapChainImageFormat_;
		VkFormat swapChainDepthFormat_;
		VkExtent2D swapChainExtent_;

		std::vector<VkFramebuffer> swapChainFramebuffers_;
		VkRenderPass renderPass_;

		std::vector<VkImage> depthImages_;
		std::vector<VkDeviceMemory> depthImageMemories_;
		std::vector<VkImageView> depthImageViews_;
		std::vector<VkImage> swapChainImages_;
		std::vector<VkImageView> swapChainImageViews_;

		PhmDevice& device_;
		VkExtent2D windowExtent_;

		VkSwapchainKHR swapChain_;
		std::shared_ptr<PhmSwapchain> oldSwapChain_;

		std::vector<VkSemaphore> imageAvailableSemaphores_;
		std::vector<VkSemaphore> renderFinishedSemaphores_;
		std::vector<VkFence> inFlightFences_;
		std::vector<VkFence> imagesInFlight_;
		size_t currentFrame_ = 0;

		void init();
		void createSwapChain();
		void createImageViews();
		void createDepthResources();
		void createRenderPass();
		void createFramebuffers();
		void createSyncObjects();

		// Helper functions
		VkSurfaceFormatKHR chooseSwapSurfaceFormat(
			const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR chooseSwapPresentMode(
			const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	};

}  // namespace phm


#endif /* PHM_SWAPCHAIN_H */
