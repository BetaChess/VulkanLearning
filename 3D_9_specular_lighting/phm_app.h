#ifndef PHM_APP_H
#define PHM_APP_H

#include <memory>
#include <vector>

#include "phm_window.h"
#include "phm_renderer.h"
#include "phm_descriptor.h"

#include "phm_manager.h"


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
		Window window_{ WIDTH, HEIGHT, "3D" };
		Device device_{ window_ };
		Renderer renderer_{ window_, device_ };

		std::unique_ptr<DescriptorPool> globalPool_{ DescriptorPool::Builder(device_)
			.setMaxSets(Swapchain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, Swapchain::MAX_FRAMES_IN_FLIGHT)
			.build() };
		ecs::Manager entityManager_{ device_, renderer_.getSwapChainRenderPass(), globalPool_.get() };
		//std::vector<Object> objects_; // TEMP
		
		void loadObjects(); // TEMP
	};
}



#endif /* PHM_APP_H */
