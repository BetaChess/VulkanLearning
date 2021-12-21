#ifndef PHM_RENDERER_H
#define PHM_RENDERER_H

#include <memory>
#include <vector>
#include <cassert>

#include "phm_window.h"
#include "phm_swapchain.h"


namespace phm
{
	class PhmRenderer
	{
	public:
		PhmRenderer(PhmWindow& window, PhmDevice& device);
		~PhmRenderer();

		PhmRenderer(const PhmRenderer&) = delete;
		PhmRenderer& operator=(const PhmRenderer&) = delete;

		inline VkRenderPass getSwapChainRenderPass() const { return swapchain_->getRenderPass(); };
		inline bool isFrameInProgress() const { return isFrameStarted_; };
		inline float getAspectRatio() const { return swapchain_->extentAspectRatio(); };

		inline VkCommandBuffer getCurrentCommandBuffer() const 
		{ 
			assert(isFrameStarted_ && "Tried to retrieve command buffer before a frame draw was initialised");
			return commandBuffers_[currentFrameIndex_]; 
		};

		inline int getFrameIndex() const 
		{ 
			assert(isFrameStarted_ && "Tried to retrieve frame index before a frame dra w was initialised");
			return currentFrameIndex_; 
		};

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

	private:
		PhmWindow& window_; // The renderer has an aggregate relation to the window an device.
		PhmDevice& device_; // ^^^
		std::unique_ptr<PhmSwapchain> swapchain_;
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