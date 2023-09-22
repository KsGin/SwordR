#include "../include/camera.h"

namespace SwordR
{
	void Camera::create(Device* device)
	{
		this->device = device;
		VkDeviceSize bufferSize = sizeof(UniformBufferPreFrame);

		uniformBuffers.resize(device->MAX_FRAMES_IN_FLIGHT);
		uniformBuffersMemory.resize(device->MAX_FRAMES_IN_FLIGHT);
		uniformBuffersMapped.resize(device->MAX_FRAMES_IN_FLIGHT);
		for (size_t i = 0; i < device->MAX_FRAMES_IN_FLIGHT; i++) {
			device->createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);
			vkMapMemory(device->logicalDevice, uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
		}
	}

	void Camera::updateCameraUBO()
	{
		ubo.view = glm::lookAt(position, lookAt, glm::vec3(0, 1, 0));
		ubo.proj = glm::perspective(fov, aspect, near, far);

		memcpy(uniformBuffersMapped[device->imageIndex], &ubo, sizeof(ubo));
	}

	void Camera::destroy()
	{
		for (size_t i = 0; i < device->MAX_FRAMES_IN_FLIGHT; i++) {
			vkDestroyBuffer(device->logicalDevice, uniformBuffers[i], nullptr);
			vkFreeMemory(device->logicalDevice, uniformBuffersMemory[i], nullptr);
		}
	}
}
