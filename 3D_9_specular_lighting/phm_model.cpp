#include "pch.h"

#include "phm_model.h"

#include "phm_utils.h"

// Libraries
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <unordered_map>


namespace std
{
	template<>
	struct hash<phm::Model::Vertex>
	{
		size_t operator()(const phm::Model::Vertex& vertex) const
		{
			size_t seed = 0;
			phm::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
			return seed;
		}
	};
}

namespace phm
{

	std::vector<VkVertexInputBindingDescription> Model::Vertex::getBindingDescriptions()
	{
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);

		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescriptions;
	}

	std::vector<VkVertexInputAttributeDescription> Model::Vertex::getAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

		attributeDescriptions.push_back({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position) });	// position
		attributeDescriptions.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) });		// color
		attributeDescriptions.push_back({ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) });	// normal
		attributeDescriptions.push_back({ 3, 0, VK_FORMAT_R32G32_SFLOAT,	offsetof(Vertex, uv) });		// uv

		return attributeDescriptions;
	}

	bool Model::Vertex::operator==(const Vertex& other) const
	{
		return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
	}


	Model::Model(Device& device, const Model::Builder& builder)
		: device_(device)
	{
		createVertexBuffers(builder.vertices);
		createIndexBuffers(builder.indices);
	}

	Model::~Model()
	{
	}

	std::unique_ptr<Model> Model::createModelFromFile(Device& device, std::string_view filePath)
	{
		Builder builder{};
		builder.loadModel(filePath);

		DebugPrint("Loading model: " << filePath);
		DebugPrint("Vertex count: " << builder.vertices.size());
		DebugPrint("Index buffer length: " << builder.indices.size());

		return std::make_unique<Model>(device, builder);
	}

	void Model::bind(VkCommandBuffer commandBuffer)
	{
		VkBuffer buffers[] = { vertexBuffer_->getBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

		if (hasIndexBuffer)
		{
			vkCmdBindIndexBuffer(commandBuffer, indexBuffer_->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
		}
	}

	void Model::draw(VkCommandBuffer commandBuffer)
	{
		if (hasIndexBuffer)
			vkCmdDrawIndexed(commandBuffer, indexCount_, 1, 0, 0, 0);
		else
			vkCmdDraw(commandBuffer, vertexCount_, 1, 0, 0);
	}

	void Model::createVertexBuffers(const std::vector<Vertex>& vertices)
	{
		vertexCount_ = static_cast<uint32_t>(vertices.size());
		assert(vertexCount_ > 2 && "Vertex Count must be at least 3");

		//VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount_;

		uint32_t vertexSize = sizeof(vertices[0]);

		// Create staging buffer for transfer to the GPU
		Buffer stagingBuffer{
			device_,
			vertexSize,
			vertexCount_,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		// Map the memory to the host and copy the data into the mapped region. 
		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)vertices.data());

		// Create the actual memory buffer on the GPU
		vertexBuffer_ = std::make_unique<Buffer>(
			device_,
			vertexSize,
			vertexCount_,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);

		// Copy the data from the staging buffer to the vertex buffer.
		device_.copyBuffer(stagingBuffer.getBuffer(), vertexBuffer_->getBuffer(), stagingBuffer.getBufferSize());
	}

	void Model::createIndexBuffers(const std::vector<uint32_t>& indices)
	{
		indexCount_ = static_cast<uint32_t>(indices.size());
		hasIndexBuffer = indexCount_ > 0;

		if (!hasIndexBuffer)
			return;

		uint32_t indexSize = sizeof(indices[0]);

		// Create staging buffer for transfer to the GPU
		Buffer stagingBuffer{
			device_,
			indexSize,
			indexCount_,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};
		
		// Map the memory to the host and copy the data into the mapped region.
		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)indices.data());

		// Create the actual memory buffer on the GPU
		indexBuffer_ = std::make_unique<Buffer>(
			device_,
			indexSize,
			indexCount_,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);

		// Copy the data from the staging buffer to the index buffer.
		device_.copyBuffer(stagingBuffer.getBuffer(), indexBuffer_->getBuffer(), stagingBuffer.getBufferSize());
	}

	void Model::Builder::loadModel(std::string_view filePath)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.data()))
		{
			throw std::runtime_error(warn + err);
		}

		vertices.clear();
		indices.clear();

		std::unordered_map<Vertex, uint32_t> uniqueVertices{};
		for (const auto& shape : shapes)
		{
			for (const auto& index : shape.mesh.indices)
			{
				Vertex vertex{};

				if (index.vertex_index >= 0)
				{
					vertex.position = {
						attrib.vertices[3 * index.vertex_index],
						attrib.vertices[3 * index.vertex_index + 1],
						attrib.vertices[3 * index.vertex_index + 2]
					};

					vertex.color = {
						attrib.colors[3 * index.vertex_index],
						attrib.colors[3 * index.vertex_index + 1],
						attrib.colors[3 * index.vertex_index + 2]
					};
				}

				if (index.normal_index >= 0)
				{
					vertex.normal = {
						attrib.normals[3 * index.normal_index],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2]
					};
				}

				if (index.texcoord_index >= 0)
				{
					vertex.uv = {
						attrib.texcoords[2 * index.texcoord_index],
						attrib.texcoords[2 * index.texcoord_index + 1],
					};
				}

				// Check if the vertex already exists
				if (uniqueVertices.count(vertex) == 0)
				{
					// If it doesn't exist, set the value (index) of the vertex in the uniqueVertices map to be the current size of the map.
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					// Push back the vertex.
					vertices.push_back(vertex);
				}

				// In all cases, push back the index of the vertex.
				indices.push_back(uniqueVertices[vertex]);
			}
		}
	}
}

phm::ecs::ModelComponent::ModelComponent(std::shared_ptr<Model>& modelPtr)
{
	model = modelPtr;
}

phm::ecs::ModelComponent::ModelComponent(Device& device, std::string_view filePath)
{
	Model::Builder builder{};
	builder.loadModel(filePath);

	DebugPrint("Loading model: " << filePath);
	DebugPrint("Vertex count: " << builder.vertices.size());
	DebugPrint("Index buffer length: " << builder.indices.size());

	model = std::make_shared<Model>(device, builder);
}
