#pragma once
#include "device.h"

namespace SwordR
{
	class Pipeline
	{
        friend class Device;
	public:
		
        enum PipelineType
        {
            BaseColor = 0,
            BaseMap = 1
        };

        struct PipelineCreateInfo
        {
        public:
            PipelineType type;
            Camera* camera;
            Texture* texture;
        };

        void create(Device* device, PipelineCreateInfo info);
        void destroy();

	private:
        Device* device;
        PipelineCreateInfo info {};

        int internalShaderCount = 2;
        std::vector<const char*> shaderNameList{
            "color",
            "texture"
        };

        VkPipeline graphicsPipeline;

        void createPipeline();
        VkShaderModule createShaderModule(const std::vector<char>& code);
        VkPipelineLayout pipelineLayout = nullptr;
        std::vector<VkDescriptorSet> descriptorSets;
        VkDescriptorSetLayout descriptorSetLayout {};
        VkDescriptorPool descriptorPool;
        void createDescriptorPool();
        void createDescriptorSets();
	};
}