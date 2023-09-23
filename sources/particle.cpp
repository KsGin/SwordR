#include "../include/particle.h"

#include <random>

namespace SwordR
{
	void ParticleSystem::create(Device* device)
	{
		this->device = device;
		shaderStorageBuffers.resize(device->MAX_FRAMES_IN_FLIGHT);
		shaderStorageBuffersMemory.resize(device->MAX_FRAMES_IN_FLIGHT);

		std::default_random_engine rndEngine((unsigned)time(nullptr));
		std::uniform_real_distribution<float> rndDist(0.0f, 1.0f);
		std::vector<Particle> particles(particleSize);
		VkDeviceSize bufferSize = sizeof(Particle) * particleSize;
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		device->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
		void* data;
		vkMapMemory(device->logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, particles.data(), (size_t)bufferSize);
		vkUnmapMemory(device->logicalDevice, stagingBufferMemory);

		for (size_t i = 0; i < device->MAX_FRAMES_IN_FLIGHT; i++) {
			device->createBuffer(bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, shaderStorageBuffers[i], shaderStorageBuffersMemory[i]);
			device->copyBuffer(stagingBuffer, shaderStorageBuffers[i], bufferSize);
		}

		VkDeviceSize uboSize = sizeof(Particle) * particleSize;
		uniformBuffers.resize(device->MAX_FRAMES_IN_FLIGHT);
		uniformBuffersMemory.resize(device->MAX_FRAMES_IN_FLIGHT);
		uniformBuffersMapped.resize(device->MAX_FRAMES_IN_FLIGHT);
		for (size_t i = 0; i < device->MAX_FRAMES_IN_FLIGHT; i++) {
			device->createBuffer(uboSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);
			vkMapMemory(device->logicalDevice, uniformBuffersMemory[i], 0, uboSize, 0, &uniformBuffersMapped[i]);
		}

		vkDestroyBuffer(device->logicalDevice, stagingBuffer, nullptr);
		vkFreeMemory(device->logicalDevice, stagingBufferMemory, nullptr);
	}

	void ParticleSystem::destroy()
	{
		for (int i = 0; i < device->MAX_FRAMES_IN_FLIGHT; ++i)
		{
			vkDestroyBuffer(device->logicalDevice, shaderStorageBuffers[i], nullptr);
			vkDestroyBuffer(device->logicalDevice, uniformBuffers[i], nullptr);
			vkFreeMemory(device->logicalDevice, shaderStorageBuffersMemory[i], nullptr);
			vkFreeMemory(device->logicalDevice, uniformBuffersMemory[i], nullptr);
		}
	}


	void ParticleSystem::updateUBO()
	{
		ubo.param.x = device->deltaTime;
		ubo.param.y = device->timeSinceStartup;
		ubo.param.z = rowSize;
		ubo.param.w = colSize;
		memcpy(uniformBuffersMapped[device->imageIndex], &ubo, sizeof(ubo));
	}
}
