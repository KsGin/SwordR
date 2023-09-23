#pragma once

#include <glm/glm.hpp>

#include "device.h"
#include "particle.h"
#include "utils.h"

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

	public:

		void create(Device* device, ParticleSystem* particleSystem);
		void destroy();
	};
}
