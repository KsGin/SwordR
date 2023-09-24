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
			alignas(16) glm::vec3 pos;
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
				attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
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
		struct Transform
		{
			glm::vec3 position;
			glm::vec3 rotation;
			glm::vec3 scale;
		};

		Transform transform {glm::vec3(0), glm::vec3(0), glm::vec3(1)};

		enum ModelType
		{
			Quad,
			Cube
		};

		ModelType type;

		std::vector<std::vector<Vertex>> verticesList{
			// Quad
			{
				{{-0.5f, -0.5f, 0}, {0, 0}, {1.0f, 0.0f, 0.0f, 1.0f}},
				{{0.5f, -0.5f, 0}, {1, 0}, {0.0f, 1.0f, 0.0f, 1.0f}},
				{{-0.5f, 0.5f, 0}, {0, 1}, {0.0f, 0.0f, 1.0f, 1.0f}},
				{{0.5f, 0.5f, 0}, {1, 1}, {1.0f, 0.0f, 1.0f, 1.0f}}
			},
			// Cube
			{
				{{-1.0f,-1.0f,-1.0f}, { 0.0f, 1.0f}, {1, 0, 0, 1}},
				{{-1.0f,-1.0f, 1.0f}, { 1.0f, 1.0f}, {0, 1, 0, 1}},
				{{-1.0f, 1.0f, 1.0f}, { 1.0f, 0.0f}, {0, 0, 1, 1}},
				{{-1.0f, 1.0f, 1.0f}, { 1.0f, 0.0f}, {1, 0, 0, 1}},
				{{-1.0f, 1.0f,-1.0f}, { 0.0f, 0.0f}, {0, 1, 0, 1}},
				{{-1.0f,-1.0f,-1.0f}, { 0.0f, 1.0f}, {0, 0, 1, 1}},
				{ {-1.0f,-1.0f,-1.0f},{ 1.0f, 1.0f}, {1, 0, 0, 1}},
				{{ 1.0f, 1.0f,-1.0f}, { 0.0f, 0.0f}, {0, 1, 0, 1}},
				{{ 1.0f,-1.0f,-1.0f}, { 0.0f, 1.0f}, {0, 0, 1, 1}},
				{{-1.0f,-1.0f,-1.0f}, { 1.0f, 1.0f}, {1, 0, 0, 1}},
				{{-1.0f, 1.0f,-1.0f}, { 1.0f, 0.0f}, {0, 1, 0, 1}},
				{{ 1.0f, 1.0f,-1.0f}, { 0.0f, 0.0f}, {0, 0, 1, 1}},
				{ {-1.0f,-1.0f,-1.0f},{ 1.0f, 0.0f}, {1, 0, 0, 1}},
				{{ 1.0f,-1.0f,-1.0f}, { 1.0f, 1.0f}, {0, 1, 0, 1}},
				{{ 1.0f,-1.0f, 1.0f}, { 0.0f, 1.0f}, {0, 0, 1, 1}},
				{{-1.0f,-1.0f,-1.0f}, { 1.0f, 0.0f}, {1, 0, 0, 1}},
				{{ 1.0f,-1.0f, 1.0f}, { 0.0f, 1.0f}, {0, 1, 0, 1}},
				{{-1.0f,-1.0f, 1.0f}, { 0.0f, 0.0f}, {0, 0, 1, 1}},
				{ {-1.0f, 1.0f,-1.0f},{ 1.0f, 0.0f}, {1, 0, 0, 1}},
				{{-1.0f, 1.0f, 1.0f}, { 0.0f, 0.0f}, {0, 1, 0, 1}},
				{{ 1.0f, 1.0f, 1.0f}, { 0.0f, 1.0f}, {0, 0, 1, 1}},
				{{-1.0f, 1.0f,-1.0f}, { 1.0f, 0.0f}, {1, 0, 0, 1}},
				{{ 1.0f, 1.0f, 1.0f}, { 0.0f, 1.0f}, {0, 1, 0, 1}},
				{{ 1.0f, 1.0f,-1.0f}, { 1.0f, 1.0f}, {0, 0, 1, 1}},
				{ { 1.0f, 1.0f,-1.0f},{ 1.0f, 0.0f}, {1, 0, 0, 1}},
				{{ 1.0f, 1.0f, 1.0f}, { 0.0f, 0.0f}, {0, 1, 0, 1}},
				{{ 1.0f,-1.0f, 1.0f}, { 0.0f, 1.0f}, {0, 0, 1, 1}},
				{{ 1.0f,-1.0f, 1.0f}, { 0.0f, 1.0f}, {1, 0, 0, 1}},
				{{ 1.0f,-1.0f,-1.0f}, { 1.0f, 1.0f}, {0, 1, 0, 1}},
				{{ 1.0f, 1.0f,-1.0f}, { 1.0f, 0.0f}, {0, 0, 1, 1}},
				{ {-1.0f, 1.0f, 1.0f},{ 0.0f, 0.0f}, {1, 0, 0, 1}},
				{{-1.0f,-1.0f, 1.0f}, { 0.0f, 1.0f}, {0, 1, 0, 1}},
				{{ 1.0f, 1.0f, 1.0f}, { 1.0f, 0.0f}, {0, 0, 1, 1}},
				{{-1.0f,-1.0f, 1.0f}, { 0.0f, 1.0f}, {1, 0, 0, 1}},
				{{ 1.0f,-1.0f, 1.0f}, { 1.0f, 1.0f}, {0, 1, 0, 1}},
				{ { 1.0f, 1.0f, 1.0f}, { 1.0f, 0.0f}, {0, 0, 1, 1}}
			}
		};

		std::vector<std::vector<uint16_t>> indicesList{
			// Quad
			{
				0, 1, 3, 3, 2, 0
			},
			{
				0,  1,  2,  3,  4,  5,
				6,  7,  8,  9,  10, 11,
				12, 13, 14, 15, 16, 17,
				18, 19, 20, 21, 22, 23,
				24, 25, 26, 27, 28, 29,
				30, 31, 32, 33, 34, 35
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
