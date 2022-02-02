/*
 * Encapsulates a vulkan buffer
 *
 * Initially based off VulkanBuffer by Sascha Willems -
 * https://github.com/SaschaWillems/Vulkan/blob/master/base/VulkanBuffer.h
 */
#include "pch.h"

#include "phm_buffer.h"

#include <cassert>
#include <cstring>

namespace phm
{
	/// <summary>
	/// Returns the minimum instance size required to be compatible with devices minOffsetAlignment
	/// </summary>
	/// <param name="instanceSize">instanceSize The size of an instance</param>
	/// <param name="minOffsetAlignment">minOffsetAlignment The minimum required alignment, in bytes, for the offset member (eg. minUniformBufferOffsetAlignment)</param>
	VkDeviceSize PhmBuffer::getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment)
	{
		if (minOffsetAlignment > 0)
		{
			return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
		}
		return instanceSize;
	}

	PhmBuffer::PhmBuffer(
		PhmDevice& device,
		VkDeviceSize instanceSize,
		uint32_t instanceCount,
		VkBufferUsageFlags usageFlags,
		VkMemoryPropertyFlags memoryPropertyFlags,
		VkDeviceSize minOffsetAlignment)
		: device_{ device },
		instanceSize_{ instanceSize },
		instanceCount_{ instanceCount },
		usageFlags_{ usageFlags },
		memoryPropertyFlags_{ memoryPropertyFlags } {
		alignmentSize_ = getAlignment(instanceSize, minOffsetAlignment);
		bufferSize_ = alignmentSize_ * instanceCount;
		device.createBuffer(bufferSize_, usageFlags, memoryPropertyFlags, buffer_, memory_);
	}

	PhmBuffer::~PhmBuffer()
	{
		unmap();
		vkDestroyBuffer(device_.device(), buffer_, nullptr);
		vkFreeMemory(device_.device(), memory_, nullptr);
	}

	/// <summary>
	/// Map a memory range of this buffer. If successful, mapped points to the specified buffer range.
	/// </summary>
	/// <param name="size">(Optional) Size of the memory range to map. Pass VK_WHOLE_SIZE to map the complete</param>
	/// <param name="offset">(Optional) Byte offset from beginning</param>
	VkResult PhmBuffer::map(VkDeviceSize size, VkDeviceSize offset)
	{
		assert(buffer_ && memory_ && "Called map on buffer before create");
		return vkMapMemory(device_.device(), memory_, offset, size, 0, &mapped_);
	}

	/// <summary>
	/// Unmap the mapped memory range
	/// </summary>
	void PhmBuffer::unmap()
	{
		if (mapped_)
		{
			vkUnmapMemory(device_.device(), memory_);
			mapped_ = nullptr;
		}
	}

	/// <summary>
	/// Copies the specified data to the mapped buffer. Default value writes whole buffer range
	/// </summary>
	/// <param name="data">Pointer to the data to copy</param>
	/// <param name="size">(Optional) Size of the data to copy. Pass VK_WHOLE_SIZE to flush the complete buffer</param>
	/// <param name="offset">(Optional) Byte offset from beginning of mapped region</param>
	void PhmBuffer::writeToBuffer(void* data, VkDeviceSize size, VkDeviceSize offset)
	{
		assert(mapped_ && "Cannot copy to unmapped buffer");

		if (size == VK_WHOLE_SIZE)
		{
			memcpy(mapped_, data, bufferSize_);
		}
		else
		{
			char* memOffset = (char*)mapped_;
			memOffset += offset;
			memcpy(memOffset, data, size);
		}
	}

	/// <summary>
	/// Flush a memory range of the buffer to make it visible to the device - Note: Only required for non-coherent memory
	/// </summary>
	/// <param name="size">(Optional) Size of the memory range to flush. Pass VK_WHOLE_SIZE to flush the complete buffer range.</param>
	/// <param name="offset">(Optional) Byte offset from beginning</param>
	VkResult PhmBuffer::flush(VkDeviceSize size, VkDeviceSize offset)
	{
		VkMappedMemoryRange mappedRange = {};
		mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		mappedRange.memory = memory_;
		mappedRange.offset = offset;
		mappedRange.size = size;
		return vkFlushMappedMemoryRanges(device_.device(), 1, &mappedRange);
	}

	/// <summary>
	/// Invalidate a memory range of the buffer to make it visible to the host - Note: Only required for non-coherent memory
	/// </summary>
	/// <param name="size">(Optional) Size of the memory range to invalidate. Pass VK_WHOLE_SIZE to invalidate the complete buffer range.</param>
	/// <param name="offset">(Optional) Byte offset from beginning</param>
	VkResult PhmBuffer::invalidate(VkDeviceSize size, VkDeviceSize offset)
	{
		VkMappedMemoryRange mappedRange = {};
		mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		mappedRange.memory = memory_;
		mappedRange.offset = offset;
		mappedRange.size = size;
		return vkInvalidateMappedMemoryRanges(device_.device(), 1, &mappedRange);
	}

	/// <summary>
	/// Create a buffer info descriptor
	/// </summary>
	/// <param name="size">(Optional) Size of the memory range of the descriptor</param>
	/// <param name="offset">(Optional) Byte offset from beginning</param>
	/// <returns>VkDescriptorBufferInfo of specified offset and range</returns>
	VkDescriptorBufferInfo PhmBuffer::descriptorInfo(VkDeviceSize size, VkDeviceSize offset)
	{
		return VkDescriptorBufferInfo{
			buffer_,
			offset,
			size,
		};
	}

	/// <summary>
	/// Copies "instanceSize" bytes of data to the mapped buffer at an offset of index * alignmentSize
	/// </summary>
	/// <param name="data">Pointer to the data to copy</param>
	/// <param name="index">Used in offset calculation</param>
	void PhmBuffer::writeToIndex(void* data, int index)
	{
		writeToBuffer(data, instanceSize_, index * alignmentSize_);
	}

	/// <summary>
	/// Flush the memory range at index * alignmentSize of the buffer to make it visible to the device
	/// </summary>
	/// <param name="index">Used in offset calculation</param>
	VkResult PhmBuffer::flushIndex(int index) { return flush(alignmentSize_, index * alignmentSize_); }

	/// <summary>
	/// Create a buffer info descriptor
	/// </summary>
	/// <param name="index">Specifies the region given by index * alignmentSize</param>
	/// <returns>VkDescriptorBufferInfo for instance at index</returns>
	VkDescriptorBufferInfo PhmBuffer::descriptorInfoForIndex(int index)
	{
		return descriptorInfo(alignmentSize_, index * alignmentSize_);
	}

	/// <summary>
	/// Invalidate a memory range of the buffer to make it visible to the host - Note: Only required for non-coherent 
	/// </summary>
	/// <param name="index">Specifies the region to invalidate: index * alignmentSize</param>
	VkResult PhmBuffer::invalidateIndex(int index)
	{
		return invalidate(alignmentSize_, index * alignmentSize_);
	}

}
