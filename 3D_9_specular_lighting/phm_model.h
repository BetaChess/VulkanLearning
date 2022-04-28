#ifndef PHM_MODEL_H
#define PHM_MODEL_H

#include <vector>
#include <memory>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>


#include "phm_device.h"
#include "phm_buffer.h"

#include "phm_component.h"

namespace phm
{
	class Model
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

		Model(Device& device, const Model::Builder& builder);
		~Model();

		Model(const Model&) = delete;
		Model& operator=(const Model&) = delete;

		// Public methods

		static std::unique_ptr<Model> createModelFromFile(Device& device, std::string_view filePath);

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

	private:
		Device& device_;

		std::unique_ptr<Buffer> vertexBuffer_;
		uint32_t vertexCount_;

		bool hasIndexBuffer = false;
		std::unique_ptr<Buffer> indexBuffer_;
		uint32_t indexCount_;

		void createVertexBuffers(const std::vector<Vertex>& vertices);
		void createIndexBuffers(const std::vector<uint32_t>& indices);
	};


	namespace ecs
	{
		class ModelComponent : public Component
		{
		public:
			ModelComponent(std::shared_ptr<Model>& model);
			ModelComponent(Device& device, std::string_view filePath);

			std::shared_ptr<Model> model{};
			glm::vec3 color{};
		};
	}
}

#endif /* PHM_MODEL_H */
