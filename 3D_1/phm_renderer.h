#ifndef PHM_RENDERER_H
#define PHM_RENDERER_H

#include <memory>
#include <vector>
#include <cassert>

#include "phm_window.h"
#include "phm_swapchain.h"


namespace phm
{
	class Renderer
	{
	public:
		Renderer(Window& window, Device& device);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

		inline VkRenderPass getSwapChainRenderPass() const { return swapchain_->getRenderPass(); };
		inline bool isFrameInProgress() const { return isFrameStarted_; };
		inline VkCommandBuffer getCurrentCommandBuffer() const 
		{ 
			assert(isFrameStarted_ && "Tried to retrieve command buffer before a frame draw was initialised");
			return commandBuffers_[currentFrameIndex_]; 
		};
		inline int getFrameIndex() const 
		{ 
			assert(isFrameStarted_ && "Tried to retrieve frame index before a frame draw was initialised");
			return currentFrameIndex_; 
		};

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

	private:
		Window& window_; // The renderer has an aggregate relation to the window an device.
		Device& device_; // ^^^
		std::unique_ptr<Swapchain> swapchain_;
		std::vector<VkCommandBuffer> commandBuffers_;

		uint32_t currentImageIndex_ = 0;
		int currentFrameIndex_ = 0;
		bool isFrameStarted_ = false;


		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapchain();
	};
}

#endif /* PHM_RENDERER_H */