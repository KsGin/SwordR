#pragma once

#include <string>
#include <stdexcept>
#include <unordered_map>

#include "vulkan/vulkan.h"
#include "device.h"

namespace SwordR
{
	class Texture
	{
		friend class GraphicsPipeline;
		friend class ComputePipeline;
	public:
		enum Usage
		{
			Graphics,
			Compute
		};

		struct CreateInfo
		{
			const char* path;
			VkFormat format;
			Usage usage;
		};

		void create(Device* device, CreateInfo info);
		void destroy();
	private:

		Device* device;
		CreateInfo info{};
		VkImage image;
		VkImageView imageView;
		VkSampler sampler;

		VkCommandBuffer beginSingleTimeCommands(Device* device);
		void endSingleTimeCommands(Device* device, VkCommandBuffer commandBuffer);
		void transitionImageLayout(Device* device, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
		void copyBufferToImage(Device* device, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

		std::unordered_map<VkImage, VkDeviceMemory> imageBufferMap{};
	};
}