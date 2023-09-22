#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <fstream>
#include <GLFW/glfw3.h>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>
#include <iostream>
#include <chrono>
#include "utils.h"

namespace SwordR
{
    struct Vertex {
        glm::vec2 pos;
        glm::vec2 uv;
        glm::vec4 color;

        static VkVertexInputBindingDescription getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            return bindingDescription;
        }

        static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
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

    struct UniformBufferObject {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    };
    

	class Device
	{
        friend class GraphicsPipeline;
        friend class Texture;

	public:
        void draw(VkBuffer vertexBuffer, VkBuffer indexBuffer, uint32_t indexCount, VkPipelineLayout layout, VkPipeline pipeline, std::vector<VkDescriptorSet> descriptorSets);
        VkBuffer createVertexBuffer(std::vector<Vertex> vertices);
        VkBuffer createIndexBuffer(std::vector<uint16_t> indices);
        void destroyVertexBuffer(VkBuffer vertexBuffer);
        void destroyIndexBuffer(VkBuffer indexBuffer);
        bool createWithWindow(GLFWwindow* window, int width, int height);
        void destroy();
        bool beginFrame();
        void endFrame();

	private:
        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
        void updateUniformBuffer(uint32_t currentImage);
        void createUniformBuffers();

        std::vector<const char*> deviceExtensions = {
            "VK_KHR_swapchain",
        };

        std::vector<const char*> ValidationLayers = {};
        bool enableValidationLayers = true;
        inline bool checkValidationLayerSupport();

		std::vector<const char*> instanceExtensions;

        std::unordered_map<VkBuffer, VkDeviceMemory> vertexMemoryMap{};
        std::unordered_map<VkBuffer, VkDeviceMemory> indexMemoryMap{};

        VkInstance instance = nullptr;
        VkSurfaceKHR surface = nullptr;
        VkSwapchainKHR swapChain = nullptr;
        VkDevice logicalDevice = nullptr;
        VkPhysicalDevice physicalDevice = nullptr;
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        VkCommandPool commandPool;
        VkQueue graphicsQueue;
        VkQueue presentQueue;
        VkExtent2D swapChainExtent;

        VkRenderPass renderPass;
        VkCommandBuffer commandBuffer;

        std::vector<VkImageView> swapChainImageViews;
        std::vector<VkImage> swapChainImages;
        std::vector<VkFramebuffer> swapChainFramebuffers;

        const int MAX_FRAMES_IN_FLIGHT = 2;
        std::vector<VkBuffer> uniformBuffers;
        std::vector<VkDeviceMemory> uniformBuffersMemory;
        std::vector<void*> uniformBuffersMapped;

        VkSemaphore imageAvailableSemaphore;
        VkSemaphore renderFinishedSemaphore;
        VkFence inFlightFence;

        uint32_t imageIndex;
	};
}

