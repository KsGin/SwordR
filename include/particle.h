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
			alignas(16) glm::vec4 position;
			alignas(16) glm::vec4 color;

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
				attributeDescriptions[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
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
			alignas(16) glm::vec4 position;
			alignas(16) glm::vec4 velocity;
			alignas(16) glm::vec4 color;
			alignas(16) glm::vec4 unused;
		};

		struct UniformBufferPreDispatch
		{
			alignas(16) glm::vec4 param;
		};

		UniformBufferPreDispatch ubo{};
		std::vector<VkBuffer> uniformBuffers;
		std::vector<VkDeviceMemory> uniformBuffersMemory;
		std::vector<void*> uniformBuffersMapped;
		std::vector<VkBuffer> shaderStorageBuffers;
		std::vector<VkDeviceMemory> shaderStorageBuffersMemory;

		const float rowSize = 256;
		const float colSize = 256;
		const float particleSize = rowSize * colSize;

		Device* device;
	public:

		void create(Device* device);
		void destroy();
		void updateUBO();
	};
}
