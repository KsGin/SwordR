#include "../include/particle.h"

#include <random>

namespace SwordR
{
	void ParticleSystem::create(Device* device)
	{
		this->device = device;
		shaderStorageBuffers.resize(device->MAX_FRAMES_IN_FLIGHT);
		shaderStorageBuffersMemory.resize(device->MAX_FRAMES_IN_FLIGHT);

		// Initialize particles
		std::default_random_engine rndEngine((unsigned)time(nullptr));
		std::uniform_real_distribution<float> rndDist(0.0f, 1.0f);

		// Initial particle positions on a circle
		std::vector<Particle> particles(maxParticleCount);
		for (auto& particle : particles) {
			float r = 0.25f * sqrt(rndDist(rndEngine));
			float theta = rndDist(rndEngine) * 2 * 3.14159265358979323846;
			float x = r * cos(theta);
			float y = r * sin(theta);
			particle.position = glm::vec2(x * 5, y * 5);
			particle.velocity = glm::normalize(glm::vec2(x, y)) * 0.25f;
			particle.color = glm::vec4(rndDist(rndEngine), rndDist(rndEngine), rndDist(rndEngine), 1.0f);
		}

		VkDeviceSize bufferSize = sizeof(Particle) * maxParticleCount;
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


		VkDeviceSize uboSize = sizeof(Particle) * maxParticleCount;
		uniformBuffers.resize(device->MAX_FRAMES_IN_FLIGHT);
		uniformBuffersMemory.resize(device->MAX_FRAMES_IN_FLIGHT);
		uniformBuffersMapped.resize(device->MAX_FRAMES_IN_FLIGHT);
		for (size_t i = 0; i < device->MAX_FRAMES_IN_FLIGHT; i++) {
			device->createBuffer(uboSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);
			vkMapMemory(device->logicalDevice, uniformBuffersMemory[i], 0, uboSize, 0, &uniformBuffersMapped[i]);
		}

		particles.clear();
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


	void ParticleSystem::updateUBO(float deltaTime)
	{
		ubo.deltaTime = deltaTime;
		ubo.rowParticleCount = device->swapChainExtent.width;
		ubo.columnParticleCount = device->swapChainExtent.height;
		ubo.count = maxParticleCount;
		memcpy(uniformBuffersMapped[device->imageIndex], &ubo, sizeof(ubo));
	}
}
