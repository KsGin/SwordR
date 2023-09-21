#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <fstream>
#include <GLFW/glfw3.h>
#include <unordered_map>
#include <glm/glm.hpp>
#include <array>
#include <iostream>
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

	class Device
	{
	public:
        enum InternalShader
        {
            Color = 0,
            Texture = 1
        };     

        void draw(VkBuffer vertexBuffer, VkBuffer indexBuffer, uint32_t indexCount, InternalShader shader);
        VkBuffer createVertexBuffer(std::vector<Vertex> vertices);
        VkBuffer createIndexBuffer(std::vector<uint16_t> indices);
        void destroyVertexBuffer(VkBuffer vertexBuffer);
        void destroyIndexBuffer(VkBuffer indexBuffer);
        bool createWithWindow(GLFWwindow* window, int width, int height);
        void destroy();
        bool beginFrame();
        void endFrame();

	private:
        std::vector<const char*> deviceExtensions = {
            "VK_KHR_swapchain",
        };

        std::vector<const char*> ValidationLayers = {};
        bool enableValidationLayers = true;
        inline bool checkValidationLayerSupport();

		std::vector<const char*> instanceExtensions;

        std::unordered_map<VkBuffer, VkDeviceMemory> vertexMemoryMap{};
        std::unordered_map<VkBuffer, VkDeviceMemory> indexMemoryMap{};

        VkDevice device = nullptr;
        VkPhysicalDevice physicalDevice = nullptr;
        VkInstance instance = nullptr;
        VkSurfaceKHR surface = nullptr;
        VkSwapchainKHR swapChain = nullptr;
        VkQueue graphicsQueue;
        VkQueue presentQueue;

        VkRenderPass renderPass;
        VkPipelineLayout pipelineLayout = nullptr;
        VkCommandPool commandPool;
        VkCommandBuffer commandBuffer;

        std::vector<VkImageView> swapChainImageViews;
        std::vector<VkImage> swapChainImages;
        std::vector<VkFramebuffer> swapChainFramebuffers;

        VkExtent2D swapChainExtent;
        VkSemaphore imageAvailableSemaphore;
        VkSemaphore renderFinishedSemaphore;
        VkFence inFlightFence;

        uint32_t imageIndex;

        int internalShaderCount = 2;
        
        std::vector<const char*> shaderNameList{
            "color",
            "texture"
        };

        std::vector<VkPipeline> shaderProgramList{
            VkPipeline {},
            VkPipeline {}
        };

        void createAllInternalPipeline();
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        VkShaderModule createShaderModule(const std::vector<char>& code);
	};
}

