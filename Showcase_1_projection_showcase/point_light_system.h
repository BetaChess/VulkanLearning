#ifndef PHM_POINT_LIGHT_SYSTEM_H
#define PHM_POINT_LIGHT_SYSTEM_H

#include <memory>
#include <vector>

#include "phm_camera.h"
#include "phm_pipeline.h"
#include "phm_object.h"
#include "phm_frame_info.h"


namespace phm
{
	class PointLightSystem
	{

	public:
		PointLightSystem(PhmDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~PointLightSystem();

		PointLightSystem(const PointLightSystem&) = delete;
		PointLightSystem& operator=(const PointLightSystem&) = delete;

		void renderObjects(const FrameInfo& frameInfo);

	private:
		PhmDevice& device_;

		std::unique_ptr<PhmPipeline> pipeline_;
		VkPipelineLayout pipelineLayout_;

		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);
	};
}

#endif /* PHM_POINT_LIGHT_SYSTEM_H */

