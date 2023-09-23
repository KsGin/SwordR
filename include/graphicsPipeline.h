#pragma once
#include "device.h"

namespace SwordR
{
	class GraphicsPipeline
	{
        friend class Device;
	public:
		
        enum GraphicsPipelineType
        {
            ParticleRenderPipeline = 0,
            ModelRenderPipeline = 1
        };

        struct GraphicsPipelineCreateInfo
        {
        public:
            GraphicsPipelineType type;
            Camera* camera;
            Texture* texture;
        };

        void create(Device* device, GraphicsPipelineCreateInfo info);
        void destroy();

	private:
        Device* device;
        GraphicsPipelineCreateInfo info {};

        int internalGraphicsCount = 2;
        std::vector<const char*> graphicsNameList{
            "color",
            "texture"
        };

        VkPipeline graphicsPipeline;

        void createPipeline();
        VkPipelineLayout pipelineLayout = nullptr;
        std::vector<VkDescriptorSet> descriptorSets;
        VkDescriptorSetLayout descriptorSetLayout {};
        VkDescriptorPool descriptorPool;
        void createDescriptorPool();
        void createDescriptorSets();
	};
}