#ifndef PHM_SIMPLE_RENDER_SYSTEM_H
#define PHM_SIMPLE_RENDER_SYSTEM_H

#include <memory>
#include <vector>

#include "phm_camera.h"
#include "phm_pipeline.h"
#include "phm_object.h"


namespace phm
{
	class SimpleRenderSystem
	{

	public:
		SimpleRenderSystem(Device& device, VkRenderPass renderPass);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void renderObjects(
			VkCommandBuffer commandBuffer, 
			const std::vector<Object>& objects, 
			const Camera& camera);

	private:
		Device& device_;

		std::unique_ptr<Pipeline> pipeline_;
		VkPipelineLayout pipelineLayout_;

		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);
	};
}

#endif /* PHM_SIMPLE_RENDER_SYSTEM_H */

