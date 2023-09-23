#pragma once
#include <array>
#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>
#include <vulkan/vulkan_core.h>
#include "device.h"

namespace SwordR
{
	class Model
	{
		friend class Device;
		friend class GraphicsPipeline;

		struct UniformBufferPreDraw
		{
			glm::mat4 model;
		};

		UniformBufferPreDraw ubo{};

		struct Vertex
		{
			alignas(16) glm::vec2 pos;
			alignas(16) glm::vec2 uv;
			alignas(16) glm::vec4 color;
			alignas(16) glm::vec4 unused;

			static VkVertexInputBindingDescription getBindingDescription()
			{
				VkVertexInputBindingDescription bindingDescription{};
				bindingDescription.binding = 0;
				bindingDescription.stride = sizeof(Vertex);
				bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
				return bindingDescription;
			}

			static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions()
			{
				std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};
				attributeDescriptions[0].binding = 0;
				attributeDescriptions[0].location = 0;
				attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
				attributeDescriptions[0].offset = offsetof(Vertex, pos);
				attributeDescriptions[1].binding = 0;
				attributeDescriptions[1].location = 1;
				attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
				attributeDescriptions[1].offset = offsetof(Vertex, uv);
				attributeDescriptions[2].binding = 0;
				attributeDescriptions[2].location = 2;
				attributeDescriptions[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
				attributeDescriptions[2].offset = offsetof(Vertex, color);
				return attributeDescriptions;
			}
		};

		void createVertexBuffer(std::vector<Vertex> vertices);
		void createIndexBuffer(std::vector<uint16_t> indices);

		Device* device;
		void updateModelUBO(float timeSinceStartup, std::vector<void*>& uniformBuffersMapped);

	public:
		enum ModelType
		{
			Quad
		};

		ModelType type;

		std::vector<std::vector<Vertex>> verticesList{
			// Quad
			{
				{{-0.5f, -0.5f}, {0, 0}, {1.0f, 0.0f, 0.0f, 1.0f}},
				{{0.5f, -0.5f}, {1, 0}, {0.0f, 1.0f, 0.0f, 1.0f}},
				{{-0.5f, 0.5f}, {0, 1}, {0.0f, 0.0f, 1.0f, 1.0f}},
				{{0.5f, 0.5f}, {1, 1}, {1.0f, 0.0f, 1.0f, 1.0f}}
			}
		};

		std::vector<std::vector<uint16_t>> indicesList{
			// Quad
			{
				0, 1, 3, 3, 2, 0
			}
		};

		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;

		void create(Device* device, ModelType type);
		void destroy();
		uint32_t getIndexCount();
	};
}
