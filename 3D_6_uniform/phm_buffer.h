#ifndef PHM_BUFFER_H
#define PHM_BUFFER_H

#include "phm_device.h"

namespace phm
{

	class PhmBuffer
	{
	public:
		PhmBuffer(
			PhmDevice& device,
			VkDeviceSize instanceSize,
			uint32_t instanceCount,
			VkBufferUsageFlags usageFlags,
			VkMemoryPropertyFlags memoryPropertyFlags,
			VkDeviceSize minOffsetAlignment = 1);
		~PhmBuffer();

		PhmBuffer(const PhmBuffer&) = delete;
		PhmBuffer& operator=(const PhmBuffer&) = delete;

		VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
		void unmap();

		void writeToBuffer(void* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
		VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
		VkDescriptorBufferInfo descriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
		VkResult invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

		void writeToIndex(void* data, int index);
		VkResult flushIndex(int index);
		VkDescriptorBufferInfo descriptorInfoForIndex(int index);
		VkResult invalidateIndex(int index);

		inline VkBuffer getBuffer() const { return buffer_; }
		inline void* getMappedMemory() const { return mapped_; }
		inline uint32_t getInstanceCount() const { return instanceCount_; }
		inline VkDeviceSize getInstanceSize() const { return instanceSize_; }
		inline VkDeviceSize getAlignmentSize() const { return alignmentSize_; }
		inline VkBufferUsageFlags getUsageFlags() const { return usageFlags_; }
		inline VkMemoryPropertyFlags getMemoryPropertyFlags() const { return memoryPropertyFlags_; }
		inline VkDeviceSize getBufferSize() const { return bufferSize_; }

	private:
		static VkDeviceSize getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);

		PhmDevice& device_;
		void* mapped_ = nullptr;
		VkBuffer buffer_ = VK_NULL_HANDLE;
		VkDeviceMemory memory_ = VK_NULL_HANDLE;

		VkDeviceSize bufferSize_;
		uint32_t instanceCount_;
		VkDeviceSize instanceSize_;
		VkDeviceSize alignmentSize_;
		VkBufferUsageFlags usageFlags_;
		VkMemoryPropertyFlags memoryPropertyFlags_;
	};

}

#endif