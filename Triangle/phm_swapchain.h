#ifndef PHM_SWAPCHAIN_H
#define PHM_SWAPCHAIN_H

#include "phm_device.h"

// vulkan headers
#include <vulkan/vulkan.h>

// stdlib
#include <string>
#include <vector>

namespace phm
{
	class PhmSwapchain
	{
	public:
		static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

		PhmSwapchain(PhmDevice& deviceRef, VkExtent2D m_windowExtent);
		~PhmSwapchain();

		// Not copyable or movable
		PhmSwapchain(const PhmSwapchain&) = delete;
		void operator=(const PhmSwapchain&) = delete;
		PhmSwapchain(PhmSwapchain&&) = delete;
		PhmSwapchain& operator=(PhmSwapchain&&) = delete;


		VkFramebuffer getFrameBuffer(int index) { return m_swapChainFramebuffers[index]; }
		VkRenderPass getRenderPass() { return m_renderPass; }
		VkImageView getImageView(int index) { return m_swapChainImageViews[index]; }
		size_t imageCount() { return m_swapChainImages.size(); }
		VkFormat getSwapChainImageFormat() { return m_swapChainImageFormat; }
		VkExtent2D getSwapChainExtent() { return m_swapChainExtent; }
		uint32_t width() { return m_swapChainExtent.width; }
		uint32_t height() { return m_swapChainExtent.height; }

		float extentAspectRatio()
		{
			return static_cast<float>(m_swapChainExtent.width) / static_cast<float>(m_swapChainExtent.height);
		}
		VkFormat findDepthFormat();

		VkResult acquireNextImage(uint32_t* imageIndex);
		VkResult submitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex);

	private:
		VkFormat m_swapChainImageFormat;
		VkExtent2D m_swapChainExtent;

		std::vector<VkFramebuffer> m_swapChainFramebuffers;
		VkRenderPass m_renderPass;

		std::vector<VkImage> m_depthImages;
		std::vector<VkDeviceMemory> m_depthImageMemories;
		std::vector<VkImageView> m_depthImageViews;
		std::vector<VkImage> m_swapChainImages;
		std::vector<VkImageView> m_swapChainImageViews;

		PhmDevice& m_device;
		VkExtent2D m_windowExtent;

		VkSwapchainKHR m_swapChain;

		std::vector<VkSemaphore> m_imageAvailableSemaphores;
		std::vector<VkSemaphore> m_renderFinishedSemaphores;
		std::vector<VkFence> m_inFlightFences;
		std::vector<VkFence> m_imagesInFlight;
		size_t m_currentFrame = 0;


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
