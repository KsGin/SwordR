#pragma once
#include <glm/glm.hpp>
#include "device.h"

namespace SwordR
{
	class ParticleSystem
	{
		friend class Device;
		friend class ComputePipeline;
		friend class GraphicsPipeline;

		struct Vertex
		{
			glm::vec2 position;
			glm::vec4 color;

			static VkVertexInputBindingDescription getBindingDescription()
			{
				VkVertexInputBindingDescription bindingDescription{};
				bindingDescription.binding = 0;
				bindingDescription.stride = sizeof(Vertex);
				bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
				return bindingDescription;
			}

			static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
				std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

				attributeDescriptions[0].binding = 0;
				attributeDescriptions[0].location = 0;
				attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
				attributeDescriptions[0].offset = offsetof(Particle, position);

				attributeDescriptions[1].binding = 0;
				attributeDescriptions[1].location = 1;
				attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
				attributeDescriptions[1].offset = offsetof(Particle, color);

				return attributeDescriptions;
			}
		};

		struct Particle
		{
			glm::vec2 position;
			glm::vec2 velocity;
			glm::vec4 color;
		};

		struct UniformBufferPreDispatch
		{
			float deltaTime;
			int rowParticleCount;
			int columnParticleCount;
			int count;
		};

		UniformBufferPreDispatch ubo{};
		std::vector<VkBuffer> uniformBuffers;
		std::vector<VkDeviceMemory> uniformBuffersMemory;
		std::vector<void*> uniformBuffersMapped;
		std::vector<VkBuffer> shaderStorageBuffers;
		std::vector<VkDeviceMemory> shaderStorageBuffersMemory;

		const int rowParticleCount = 128;
		const int columnParticleCount = 128;
		const int maxParticleCount = rowParticleCount * columnParticleCount;

		Device* device;
	public:

		void create(Device* device);
		void destroy();
		void updateUBO(float deltaTime);
	};
}
