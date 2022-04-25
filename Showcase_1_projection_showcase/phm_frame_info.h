#ifndef PHM_FRAME_INFO_H
#define PHM_FRAME_INFO_H

#include "phm_camera.h"

#include <vulkan/vulkan.h>


namespace phm
{
	struct FrameInfo
	{
		int frameIndex;
		float deltaTime;
		VkCommandBuffer commandBuffer;
		PhmCamera& camera;
		VkDescriptorSet globalDescriptorSet;
	};
}

#endif /* PHM_FRAME_INFO_H */
