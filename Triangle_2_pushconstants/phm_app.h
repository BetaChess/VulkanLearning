#ifndef PHM_APP_H
#define PHM_APP_H

#include <memory>
#include <vector>

#include "phm_window.h"
#include "phm_pipeline.h"
#include "phm_swapchain.h"
#include "phm_object.h"


namespace phm
{
	class Application
	{

	public:
		static constexpr size_t WIDTH = 800;
		static constexpr size_t HEIGHT = 600;

		Application();
		~Application();

		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;

		void run();

	private:
		Window window_{ WIDTH, HEIGHT, "Triangle" };
		Device device_{ window_ };
		std::unique_ptr<Swapchain> swapchain_;
		std::unique_ptr<Pipeline> pipeline_;
		VkPipelineLayout pipelineLayout_;
		std::vector<VkCommandBuffer> commandBuffers_;
		std::vector<Object> objects_; // TEMP

		void loadObjects(); // TEMP
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void freeCommandBuffers();
		void drawFrame();
		void recreateSwapchain();
		void recordCommandBuffer(size_t imageIndex);
		void renderObjects(VkCommandBuffer commandBuffer);

	};
}



#endif /* PHM_APP_H */
