#ifndef PHM_APP_H
#define PHM_APP_H

#include "phm_window.h"
#include "phm_pipeline.h"
#include "phm_swapchain.h"
#include "phm_model.h"

#include <memory>
#include <vector>

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
		PhmWindow window_{ WIDTH, HEIGHT, "Triangle" };
		PhmDevice device_{ window_ };
		std::unique_ptr<PhmSwapchain> swapchain_;
		std::unique_ptr<PhmPipeline> pipeline_;
		VkPipelineLayout pipelineLayout_;
		std::vector<VkCommandBuffer> commandBuffers_;
		std::unique_ptr<PhmModel> model_; // TEMP

		void loadModels(); // TEMP
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void freeCommandBuffers();
		void drawFrame();
		void recreateSwapchain();
		void recordCommandBuffer(size_t imageIndex);

	};
}



#endif /* PHM_APP_H */
