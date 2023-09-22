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
		friend class Pipeline;

	public:
		void create(Device* device, const char* path);
		void destroy();
	private:

		Device* device;
		VkImage image;
		VkImageView imageView;
		VkSampler sampler;

		VkImage CreateImageFromPath(Device* device, const char* path);
		void releaseImage(Device* device, VkImage image);
		VkImageView createImageView(Device* device, VkImage image, VkFormat format);
		void releaseImageView(Device* device, VkImageView imageView);
		VkSampler createSampler(Device* device);
		void releaseSampler(Device* device, VkSampler sampler);

		VkCommandBuffer beginSingleTimeCommands(Device* device);
		void endSingleTimeCommands(Device* device, VkCommandBuffer commandBuffer);
		void copyBuffer(Device* device, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		void transitionImageLayout(Device* device, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
		void copyBufferToImage(Device* device, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

		std::unordered_map<VkImage, VkDeviceMemory> imageBufferMap{};
	};
}