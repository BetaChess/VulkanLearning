#include "pch.h"

#include "phm_manager.h"
#include "phm_pointLightComponent.h"

#include <algorithm>

namespace phm
{
	namespace ecs
	{
		Manager::Manager(Device& device, VkRenderPass renderPass, DescriptorPool* descriptorPool) :
			device_(device),
			globalSetLayout_{ DescriptorSetLayout::Builder(device_)
				.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
				.build() },
			simpleRenderSystem_{ device, renderPass, globalSetLayout_->getDescriptorSetLayout() },
			pointLightSystem_{ device, renderPass, globalSetLayout_->getDescriptorSetLayout() }
		{
			// Initialize uniform buffers
			for (auto& bufferPtr : uniformBuffers)
			{
				bufferPtr = std::make_unique<Buffer>(device_,
					sizeof(GlobalUbo),
					Swapchain::MAX_FRAMES_IN_FLIGHT,
					VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
					device_.properties.limits.minUniformBufferOffsetAlignment);
				bufferPtr->map();
			}

			// initialize descriptor sets
			for (size_t i = 0; i < globalDescriptorSets_.size(); i++)
			{
				auto bufferInfo = uniformBuffers[i]->descriptorInfo();
				DescriptorWriter(*globalSetLayout_, *descriptorPool)
					.writeBuffer(0, &bufferInfo)
					.build(globalDescriptorSets_[i]);
			}
		}

		void Manager::update(const FrameInfo& frameInfo, const Renderer& renderer, GLFWwindow* window)
		{
			cameraController_.moveInPlaneXZ(window, frameInfo.deltaTime, *viewerEntity_);
			activeCamera_->setViewYXZ(viewerEntity_->transform.translation, viewerEntity_->transform.rotation);

			float aspect = renderer.getAspectRatio();
			activeCamera_->setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 100.0f);

			for (auto& e : entities_)
				e->update();

			// Update global uniform buffer 
			// (THIS SHOULD ALWAYS BE DONE LAST, AS ENTITIES CAN CHANGE THE STATE OF THE UPDATED DATA, MAKING THE UBO BE OUT OF DATE FOR THE FRAME IN QUESTION)
			GlobalUbo ubo{};
			
			// Update the lights in the scene
			for (const auto& entity : entities_)
			{
				if (entity->hasComponent<PointlightComponent>())
				{
					assert(activeLights_ != MAX_LIGHTS && "Too many lights in the scene!");

					const auto& pointLight = entity->getComponent<PointlightComponent>();
					ubo.pointLights[ubo.activeLights].color = pointLight.getColorIntensity();
					ubo.pointLights[ubo.activeLights].position = { entity->transform.translation, pointLight.getRadius() };
					ubo.activeLights++;
				}
			}
			activeLights_ = static_cast<uint32_t>(ubo.activeLights);
			
			ubo.projection = activeCamera_->getProjection();
			ubo.view = activeCamera_->getView();
			ubo.inverseView = activeCamera_->getInverseView();
			uniformBuffers[frameInfo.frameIndex]->writeToBuffer(&ubo);
			uniformBuffers[frameInfo.frameIndex]->flush();
		}

		void Manager::render(const FrameInfo& frameInfo, const Renderer& renderer) const
		{
			std::vector<Entity*> simpleEntities;
			for (const auto& entity : entities_)
			{
				if (entity->hasComponent<ModelComponent>())
					simpleEntities.push_back(entity.get());
			}

			simpleRenderSystem_.renderObjects(frameInfo, simpleEntities, &globalDescriptorSets_[frameInfo.frameIndex]);
			pointLightSystem_.renderObjects(frameInfo, &globalDescriptorSets_[frameInfo.frameIndex], activeLights_);
		}

		void Manager::refresh()
		{
			entities_.erase(std::remove_if(entities_.begin(), entities_.end(),
				[](const std::unique_ptr<Entity>& e)
				{
					return e->isAlive();
				}),
				entities_.end());
		}

		Entity& Manager::addEntity()
		{
			Entity* e = new Entity();
			std::unique_ptr<Entity> uPtr{ e };
			entities_.emplace_back(std::move(uPtr));
			return *e;
		}
	}
}

