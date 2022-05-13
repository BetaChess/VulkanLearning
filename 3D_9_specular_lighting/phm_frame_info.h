#ifndef PHM_FRAME_INFO_H
#define PHM_FRAME_INFO_H

#include "phm_camera.h"

#include <vulkan/vulkan.h>


namespace phm
{
#define MAX_LIGHTS 20
	
	struct PointLight
	{
		glm::vec4 position; // w is RADIUS
		glm::vec4 color; // w is intensity
	};

	struct GlobalUbo
	{
		glm::mat4 projection{ 1.0f };
		glm::mat4 view{ 1.0f };
		glm::mat4 inverseView{ 1.0f };
		glm::vec4 ambientLightColor{ 1.0f, 1.0f, 1.0f, 0.02f };
		PointLight pointLights[MAX_LIGHTS];
		int activeLights = 0;
	};

	struct FrameInfo
	{
		int frameIndex;
		float deltaTime;
		VkCommandBuffer commandBuffer;
		Camera& camera;
	};
}

#endif /* PHM_FRAME_INFO_H */
