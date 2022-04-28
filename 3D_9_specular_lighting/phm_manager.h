#ifndef PHM_MANAGER_H
#define PHM_MANAGER_H

#include <memory>

#include "phm_entity.h"

#include "phm_device.h"
#include "phm_frame_info.h"
#include "phm_renderer.h"
#include "phm_descriptor.h"
#include "phm_buffer.h"

#include "simple_render_system.h"
#include "point_light_system.h"

#include "phm_keyboardController.h"

namespace phm
{
	namespace ecs
	{

		class Manager
		{
		public:
			Manager(Device& device, VkRenderPass renderPass, DescriptorPool* descriptorPool);

			void update(const FrameInfo& frameInfo, const Renderer& renderer, GLFWwindow* window);
			void render(const FrameInfo& frameInfo, const Renderer& renderer) const;

			void refresh();

			Entity& addEntity();

			inline void setCamera(Camera* camera)
			{
				assert(camera != nullptr && "Camera is nullptr!");
				activeCamera_ = camera;
			};
			inline void setViewerEntity(Entity* entity)
			{
				assert(entity != nullptr && "Viewer entity is nullptr!");
				viewerEntity_ = entity;
			};

		private:
			std::vector<std::unique_ptr<Entity>> entities_{};

			// Scene information
			Camera* activeCamera_;
			Entity* viewerEntity_;

			uint32_t activeLights_ = 0;

			// Vulkan references
			Device& device_;

			// Uniform buffers
			std::vector<std::unique_ptr<Buffer>> uniformBuffers{ Swapchain::MAX_FRAMES_IN_FLIGHT };

			// Descriptor sets
			std::unique_ptr<DescriptorSetLayout> globalSetLayout_;

			std::vector<VkDescriptorSet> globalDescriptorSets_{ Swapchain::MAX_FRAMES_IN_FLIGHT };

			// Render Systems
			SimpleRenderSystem simpleRenderSystem_;
			PointLightSystem pointLightSystem_;

			// Scene update members
			KeyboardController cameraController_{};
		};
	}
}

#endif /* PHM_MANAGER_H */