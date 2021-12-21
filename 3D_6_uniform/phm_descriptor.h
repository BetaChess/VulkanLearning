#ifndef PHM_DESCRIPTOR_H
#define PHM_DESCRIPTOR_H

#include "phm_device.h"

#include <memory>
#include <unordered_map>
#include <vector>

namespace phm
{

	class PhmDescriptorSetLayout
	{
	public:
		class Builder
		{
		public:
			Builder(PhmDevice& device) : device_{ device } {}

			Builder& addBinding(
				uint32_t binding,
				VkDescriptorType descriptorType,
				VkShaderStageFlags stageFlags,
				uint32_t count = 1);
			std::unique_ptr<PhmDescriptorSetLayout> build() const;

		private:
			PhmDevice& device_;
			std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
		};

		PhmDescriptorSetLayout(
			PhmDevice& device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
		~PhmDescriptorSetLayout();
		PhmDescriptorSetLayout(const PhmDescriptorSetLayout&) = delete;
		PhmDescriptorSetLayout& operator=(const PhmDescriptorSetLayout&) = delete;

		inline VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout_; }

	private:
		PhmDevice& device_;
		VkDescriptorSetLayout descriptorSetLayout_;
		std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings_;

		friend class PhmDescriptorWriter;
	};


	class PhmDescriptorPool
	{
	public:
		class Builder
		{
		public:
			Builder(PhmDevice& device) : device_{ device } {}

			Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
			Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
			Builder& setMaxSets(uint32_t count);
			std::unique_ptr<PhmDescriptorPool> build() const;

		private:
			PhmDevice& device_;
			std::vector<VkDescriptorPoolSize> poolSizes_{};
			uint32_t maxSets_ = 1000;
			VkDescriptorPoolCreateFlags poolFlags_ = 0;
		};

		PhmDescriptorPool(
			PhmDevice& device,
			uint32_t maxSets,
			VkDescriptorPoolCreateFlags poolFlags,
			const std::vector<VkDescriptorPoolSize>& poolSizes);
		~PhmDescriptorPool();
		PhmDescriptorPool(const PhmDescriptorPool&) = delete;
		PhmDescriptorPool& operator=(const PhmDescriptorPool&) = delete;

		bool allocateDescriptorSet(
			const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

		void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

		void resetPool();

	private:
		PhmDevice& device_;
		VkDescriptorPool descriptorPool_;

		friend class PhmDescriptorWriter;
	};

	class PhmDescriptorWriter
	{
	public:
		PhmDescriptorWriter(PhmDescriptorSetLayout& setLayout, PhmDescriptorPool& pool);

		PhmDescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
		PhmDescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

		bool build(VkDescriptorSet& set);
		void overwrite(VkDescriptorSet& set);

	private:
		PhmDescriptorSetLayout& setLayout_;
		PhmDescriptorPool& pool_;
		std::vector<VkWriteDescriptorSet> writes_;
	};
}
#endif