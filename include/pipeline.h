#pragma once
#include "device.h"

namespace SwordR
{
    enum InternalShaderType
    {
        Color = 0,
        Texture = 1
    };

	class GraphicsPipeline
	{
	public:
        void create(Device* device);
        void destroy();
        VkPipeline getPipeline(InternalShaderType type);

        VkPipelineLayout pipelineLayout = nullptr;
        std::vector<VkDescriptorSet> descriptorSets;
	private:
        Device* device;

        int internalShaderCount = 2;
        std::vector<const char*> shaderNameList{
            "color",
            "texture"
        };

        std::vector<VkPipeline> graphicsPipelineList{
            VkPipeline {},
            VkPipeline {}
        };

        void createAllInternalPipeline();
        VkShaderModule createShaderModule(const std::vector<char>& code);


        VkDescriptorSetLayout descriptorSetLayout;

        VkDescriptorPool descriptorPool;
        void createDescriptorPool();
        void createDescriptorSets();

	};
}