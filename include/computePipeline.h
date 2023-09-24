#pragma once

#include <glm/glm.hpp>

#include "device.h"
#include "particle.h"
#include "utils.h"
#include "texture.h"

namespace SwordR
{
	class ComputePipeline
	{
		friend class Device;

		Device* device;
		VkPipeline computePipeline;
		VkPipelineLayout computePipelineLayout;
		VkDescriptorSetLayout computeDescriptorSetLayout;
		std::vector<VkDescriptorSet> computeDescriptorSets;
		VkDescriptorPool computeDescriptorPool;

		struct ComputePipelineCreateInfo
		{
		public:
			ParticleSystem* particle;
			Texture* texture;
		};

	public:

		void create(Device* device, ComputePipelineCreateInfo info);
		void destroy();
	};
}
