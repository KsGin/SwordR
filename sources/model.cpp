#include "../include/model.h"

#include <chrono>
#include <glm/ext/matrix_transform.hpp>

namespace SwordR
{
    void Model::createVertexBuffer(std::vector<Vertex> vertices) {
        device->createBuffer(sizeof(vertices[0]) * vertices.size(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vertexBuffer, vertexBufferMemory);

        void* data;
        vkMapMemory(device->logicalDevice, vertexBufferMemory, 0, sizeof(vertices[0]) * vertices.size(), 0, &data);
        memcpy(data, vertices.data(), (size_t)sizeof(vertices[0]) * vertices.size());
        vkUnmapMemory(device->logicalDevice, vertexBufferMemory);
    }

    void Model::createIndexBuffer(std::vector<uint16_t> indices) {

        device->createBuffer(sizeof(indices[0]) * indices.size(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, indexBuffer, indexBufferMemory);

        void* data;
        vkMapMemory(device->logicalDevice, indexBufferMemory, 0, sizeof(indices[0]) * indices.size(), 0, &data);
        memcpy(data, indices.data(), (size_t)sizeof(indices[0]) * indices.size());
        vkUnmapMemory(device->logicalDevice, indexBufferMemory);
    }

    void Model::destroy() {
        vkFreeMemory(device->logicalDevice, vertexBufferMemory, nullptr);
        vkDestroyBuffer(device->logicalDevice, vertexBuffer, nullptr);
        vkFreeMemory(device->logicalDevice, indexBufferMemory, nullptr);
        vkDestroyBuffer(device->logicalDevice, indexBuffer, nullptr);

        for (size_t i = 0; i < device->MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroyBuffer(device->logicalDevice, uniformBuffers[i], nullptr);
            vkFreeMemory(device->logicalDevice, uniformBuffersMemory[i], nullptr);
        }
    }

    void Model::updateModelUBO()
    {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
        UniformBufferPreDraw ubo{};
        ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        memcpy(uniformBuffersMapped[device->imageIndex], &ubo, sizeof(ubo));
    }

    void Model::create(Device* device, ModelType type)
    {
        this->device = device;
        this->type = type;
        createVertexBuffer(verticesList[static_cast<int>(type)]);
        createIndexBuffer(indicesList[static_cast<int>(type)]);

        VkDeviceSize bufferSize = sizeof(UniformBufferPreDraw);

        uniformBuffers.resize(device->MAX_FRAMES_IN_FLIGHT);
        uniformBuffersMemory.resize(device->MAX_FRAMES_IN_FLIGHT);
        uniformBuffersMapped.resize(device->MAX_FRAMES_IN_FLIGHT);
        for (size_t i = 0; i < device->MAX_FRAMES_IN_FLIGHT; i++) {
            device->createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);
            vkMapMemory(device->logicalDevice, uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
        }
    }

    uint32_t Model::getIndexCount()
    {
        return (uint32_t) indicesList[static_cast<int>(type)].size();
    }
}
