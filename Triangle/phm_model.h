#ifndef PHM_MODEL_H
#define PHM_MODEL_H

#include "phm_device.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>

namespace phm
{
	class PhmModel
	{
	public:

		// Public member variables
		struct Vertex
		{
			glm::vec2 pos;
			glm::vec3 col;

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
		};

		// Constructors

		PhmModel(PhmDevice& device, const std::vector<Vertex>& vertices);
		~PhmModel();

		PhmModel(const PhmModel&) = delete;
		PhmModel& operator=(const PhmModel&) = delete;

		// Public methods

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

	private:
		PhmDevice& m_device;
		VkBuffer m_vertexBuffer;
		VkDeviceMemory m_vertexBufferMemory;
		uint32_t m_vertexCount;

		void createVertexBuffers(const std::vector<Vertex>& vertices);
	};
}

#endif /* PHM_MODEL_H */
