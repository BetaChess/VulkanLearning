#include "pch.h"

#include "phm_descriptor.h"


namespace phm
{
    // *************** Descriptor Set Layout Builder *********************

    PhmDescriptorSetLayout::Builder& PhmDescriptorSetLayout::Builder::addBinding(
        uint32_t binding,
        VkDescriptorType descriptorType,
        VkShaderStageFlags stageFlags,
        uint32_t count)
    {
        assert(bindings.count(binding) == 0 && "Binding already in use");
        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding = binding;
        layoutBinding.descriptorType = descriptorType;
        layoutBinding.descriptorCount = count;
        layoutBinding.stageFlags = stageFlags;
        bindings[binding] = layoutBinding;
        return *this;
    }

    std::unique_ptr<PhmDescriptorSetLayout> PhmDescriptorSetLayout::Builder::build() const
    {
        return std::make_unique<PhmDescriptorSetLayout>(device_, bindings);
    }

    // *************** Descriptor Set Layout *********************

    PhmDescriptorSetLayout::PhmDescriptorSetLayout(
        PhmDevice& device_, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
        : device_{ device_ }, bindings_{ bindings } {
        std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
        for (auto kv : bindings)
        {
            setLayoutBindings.push_back(kv.second);
        }

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
        descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
        descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

        if (vkCreateDescriptorSetLayout(
            device_.device(),
            &descriptorSetLayoutInfo,
            nullptr,
            &descriptorSetLayout_) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    PhmDescriptorSetLayout::~PhmDescriptorSetLayout()
    {
        vkDestroyDescriptorSetLayout(device_.device(), descriptorSetLayout_, nullptr);
    }

    // *************** Descriptor Pool Builder *********************

    PhmDescriptorPool::Builder& PhmDescriptorPool::Builder::addPoolSize(
        VkDescriptorType descriptorType, uint32_t count)
    {
        poolSizes_.push_back({ descriptorType, count });
        return *this;
    }

    PhmDescriptorPool::Builder& PhmDescriptorPool::Builder::setPoolFlags(
        VkDescriptorPoolCreateFlags flags)
    {
        poolFlags_ = flags;
        return *this;
    }
    PhmDescriptorPool::Builder& PhmDescriptorPool::Builder::setMaxSets(uint32_t count)
    {
        maxSets_ = count;
        return *this;
    }

    std::unique_ptr<PhmDescriptorPool> PhmDescriptorPool::Builder::build() const
    {
        return std::make_unique<PhmDescriptorPool>(device_, maxSets_, poolFlags_, poolSizes_);
    }

    // *************** Descriptor Pool *********************

    PhmDescriptorPool::PhmDescriptorPool(
        PhmDevice& device_,
        uint32_t maxSets,
        VkDescriptorPoolCreateFlags poolFlags,
        const std::vector<VkDescriptorPoolSize>& poolSizes)
        : device_{ device_ }
    {
        VkDescriptorPoolCreateInfo descriptorPoolInfo{};
        descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        descriptorPoolInfo.pPoolSizes = poolSizes.data();
        descriptorPoolInfo.maxSets = maxSets;
        descriptorPoolInfo.flags = poolFlags;

        if (vkCreateDescriptorPool(device_.device(), &descriptorPoolInfo, nullptr, &descriptorPool_) !=
            VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    PhmDescriptorPool::~PhmDescriptorPool()
    {
        vkDestroyDescriptorPool(device_.device(), descriptorPool_, nullptr);
    }

    bool PhmDescriptorPool::allocateDescriptorSet(
        const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const
    {
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool_;
        allocInfo.pSetLayouts = &descriptorSetLayout;
        allocInfo.descriptorSetCount = 1;

        // Might want to create a "DescriptorPoolManager" class that handles this case, and builds
        // a new pool whenever an old pool fills up. But this is beyond our current scope
        if (vkAllocateDescriptorSets(device_.device(), &allocInfo, &descriptor) != VK_SUCCESS)
        {
            return false;
        }
        return true;
    }

    void PhmDescriptorPool::freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const
    {
        vkFreeDescriptorSets(
            device_.device(),
            descriptorPool_,
            static_cast<uint32_t>(descriptors.size()),
            descriptors.data());
    }

    void PhmDescriptorPool::resetPool()
    {
        vkResetDescriptorPool(device_.device(), descriptorPool_, 0);
    }

    // *************** Descriptor Writer *********************

    PhmDescriptorWriter::PhmDescriptorWriter(PhmDescriptorSetLayout& setLayout, PhmDescriptorPool& pool)
        : setLayout_{ setLayout }, pool_{ pool } {}

    PhmDescriptorWriter& PhmDescriptorWriter::writeBuffer(
        uint32_t binding, VkDescriptorBufferInfo* bufferInfo)
    {
        assert(setLayout_.bindings_.count(binding) == 1 && "Layout does not contain specified binding");

        auto& bindingDescription = setLayout_.bindings_[binding];

        assert(
            bindingDescription.descriptorCount == 1 &&
            "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pBufferInfo = bufferInfo;
        write.descriptorCount = 1;

        writes_.push_back(write);
        return *this;
    }

    PhmDescriptorWriter& PhmDescriptorWriter::writeImage(
        uint32_t binding, VkDescriptorImageInfo* imageInfo)
    {
        assert(setLayout_.bindings_.count(binding) == 1 && "Layout does not contain specified binding");

        auto& bindingDescription = setLayout_.bindings_[binding];

        assert(
            bindingDescription.descriptorCount == 1 &&
            "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pImageInfo = imageInfo;
        write.descriptorCount = 1;

        writes_.push_back(write);
        return *this;
    }

    bool PhmDescriptorWriter::build(VkDescriptorSet& set)
    {
        bool success = pool_.allocateDescriptorSet(setLayout_.getDescriptorSetLayout(), set);
        if (!success)
        {
            return false;
        }
        overwrite(set);
        return true;
    }

    void PhmDescriptorWriter::overwrite(VkDescriptorSet& set)
    {
        for (auto& write : writes_)
        {
            write.dstSet = set;
        }
        vkUpdateDescriptorSets(pool_.device_.device(), writes_.size(), writes_.data(), 0, nullptr);
    }
}