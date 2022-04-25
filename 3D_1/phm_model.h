#ifndef PHM_MODEL_H
#define PHM_MODEL_H

#include "phm_device.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>

namespace phm
{
	class Model
	{
	public:

		// Public member variables
		struct Vertex
		{
			glm::vec3 position;
			glm::vec3 color;

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
		};

		// Constructors

		Model(Device& device, const std::vector<Vertex>& vertices);
		~Model();

		Model(const Model&) = delete;
		Model& operator=(const Model&) = delete;

		// Public methods

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

	private:
		Device& device_;
		VkBuffer vertexBuffer_;
		VkDeviceMemory vertexBufferMemory_;
		uint32_t vertexCount_;

		void createVertexBuffers(const std::vector<Vertex>& vertices);
	};
}

#endif /* PHM_MODEL_H */
