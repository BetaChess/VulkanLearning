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
		static constexpr size_t WIDTH = 800*2;
		static constexpr size_t HEIGHT = 600*2;

		Application();
		~Application();

		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;

		void run();

	private:
		PhmWindow m_window{ WIDTH, HEIGHT, "Triangle" };
		PhmDevice m_device{ m_window };
		PhmSwapchain m_swapchain{ m_device, m_window.getExtent() };
		std::unique_ptr<PhmPipeline> m_pipeline;
		VkPipelineLayout m_pipelineLayout;
		std::vector<VkCommandBuffer> m_commandBuffers;
		std::unique_ptr<PhmModel> m_model; // TEMP

		void loadModels(); // TEMP
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void drawFrame();


	};
}



#endif /* PHM_APP_H */
