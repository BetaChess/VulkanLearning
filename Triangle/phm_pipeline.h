#ifndef PHM_PIPELINE_H
#define PHM_PIPELINE_H

#include "phm_device.h"

#include <string>
#include <vector>

namespace phm
{
	struct PipelineConfigInfo
	{
		PipelineConfigInfo() = default;
		PipelineConfigInfo(const PipelineConfigInfo&) = delete;
		PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

		VkViewport viewport;
		VkRect2D scissor;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
		VkPipelineRasterizationStateCreateInfo rasterizationInfo{};
		VkPipelineMultisampleStateCreateInfo multisampleInfo{};
		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};

	class PhmPipeline
	{
	public:
		// Constructor(s)
		PhmPipeline(
			PhmDevice& device, 
			const std::string& vertFilePath, 
			const std::string& fragFilePath, 
			const PipelineConfigInfo& configInfo);

		~PhmPipeline();

		// Not copyable or movable
		PhmPipeline(const PhmPipeline&) = delete;
		void operator=(const PhmPipeline&) = delete;
		PhmPipeline(PhmPipeline&&) = delete;
		PhmPipeline& operator=(PhmPipeline&&) = delete;

		void bind(VkCommandBuffer commandBuffer);

		// Static methods
		static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo, uint32_t width, uint32_t height);

	private:
		// Private member variables
		PhmDevice& m_device; // Pipeline has an aggregate relation to the device.
		VkPipeline m_graphicsPipeline;
		VkShaderModule m_vertexShaderModule;
		VkShaderModule m_fragmentShaderModule;


		// Private methods
		static std::vector<char> readFile(const std::string& filename);

		void createGraphicsPipeline(
			const std::string& vertFilePath, 
			const std::string& fragFilePath, 
			const PipelineConfigInfo& configInfo);

		void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

	};

}


#endif /* PHM_PIPELINE_H */
