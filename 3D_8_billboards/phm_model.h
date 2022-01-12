#ifndef PHM_MODEL_H
#define PHM_MODEL_H

#include "phm_device.h"
#include "phm_buffer.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
#include <memory>

namespace phm
{
	class PhmModel
	{
	public:

		// Public structs
		struct Vertex
		{
			glm::vec3 position{};
			glm::vec3 color{};
			glm::vec3 normal{};
			glm::vec2 uv{};

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

			bool operator==(const Vertex& other) const;
		};

		// Temporary helper object for construction of models
		struct Builder
		{
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};

			void loadModel(std::string_view filePath);
		};


		// Constructors

		PhmModel(PhmDevice& device, const PhmModel::Builder& builder);
		~PhmModel();

		PhmModel(const PhmModel&) = delete;
		PhmModel& operator=(const PhmModel&) = delete;

		// Public methods

		static std::unique_ptr<PhmModel> createModelFromFile(PhmDevice& device, std::string_view filePath);

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

	private:
		PhmDevice& device_;

		std::unique_ptr<PhmBuffer> vertexBuffer_;
		uint32_t vertexCount_;

		bool hasIndexBuffer = false;
		std::unique_ptr<PhmBuffer> indexBuffer_;
		uint32_t indexCount_;

		void createVertexBuffers(const std::vector<Vertex>& vertices);
		void createIndexBuffers(const std::vector<uint32_t>& indices);
	};
}

#endif /* PHM_MODEL_H */
