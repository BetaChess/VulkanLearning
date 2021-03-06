#ifndef PHM_FRUSTUM_RENDER_SYSTEM_H
#define PHM_FRUSTUM_RENDER_SYSTEM_H

#include <memory>
#include <vector>

#include "phm_camera.h"
#include "phm_pipeline.h"
#include "phm_object.h"
#include "phm_frame_info.h"


namespace phm
{
	class FrustumRenderSystem
	{

	public:
		FrustumRenderSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~FrustumRenderSystem();

		FrustumRenderSystem(const FrustumRenderSystem&) = delete;
		FrustumRenderSystem& operator=(const FrustumRenderSystem&) = delete;

		void renderObjects(
			const FrameInfo& frameInfo, 
			const std::vector<Object>& objects);

	private:
		Device& device_;

		std::unique_ptr<Pipeline> pipeline_;
		VkPipelineLayout pipelineLayout_;

		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);
	};
}

#endif /* PHM_FRUSTUM_RENDER_SYSTEM_H */

