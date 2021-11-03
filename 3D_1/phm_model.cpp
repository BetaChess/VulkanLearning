#include "phm_model.h"

namespace phm
{

	std::vector<VkVertexInputBindingDescription> PhmModel::Vertex::getBindingDescriptions()
	{
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);

		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescriptions;
	}

	std::vector<VkVertexInputAttributeDescription> PhmModel::Vertex::getAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);

		// position
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, position);

		// Color
		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		return attributeDescriptions;
	}


	PhmModel::PhmModel(PhmDevice& device, const std::vector<Vertex>& vertices)
		: device_(device)
	{
		createVertexBuffers(vertices);
	}

	PhmModel::~PhmModel()
	{
		vkDestroyBuffer(device_.device(), vertexBuffer_, nullptr);
		vkFreeMemory(device_.device(), vertexBufferMemory_, nullptr);
	}

	void PhmModel::bind(VkCommandBuffer commandBuffer)
	{
		VkBuffer buffers[] = { vertexBuffer_ };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
	}

	void PhmModel::draw(VkCommandBuffer commandBuffer)
	{
		vkCmdDraw(commandBuffer, vertexCount_, 1, 0, 0);
	}

	void PhmModel::createVertexBuffers(const std::vector<Vertex>& vertices)
	{
		vertexCount_ = static_cast<uint32_t>(vertices.size());
		assert(vertexCount_ > 2 && "Vertex Count must be at least 3");

		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount_;

		device_.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			vertexBuffer_,
			vertexBufferMemory_
		);

		void* data;
		vkMapMemory(device_.device(), vertexBufferMemory_, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(device_.device(), vertexBufferMemory_);
	}

}
