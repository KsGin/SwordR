#pragma once

#include <chrono>
#include <vulkan/vulkan.h>
#include <vector>
#include <array>
#include <GLFW/glfw3.h>

namespace SwordR
{
	class Device
	{
        friend class Pipeline;
        friend class Texture;
        friend class Camera;
        friend class Model;

	public:
        void draw(Model* model, Pipeline* pipeline);
        bool createWithWindow(GLFWwindow* window, int width, int height);
        void destroy();
        void beginFrame();
        void endFrame();
        void waitFenceAndReset();

	private:
        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

        std::vector<const char*> deviceExtensions = {
            "VK_KHR_swapchain",
        };

        std::vector<const char*> ValidationLayers = {};
        bool enableValidationLayers = true;
        inline bool checkValidationLayerSupport();

		std::vector<const char*> instanceExtensions;

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

        VkSemaphore imageAvailableSemaphore;
        VkSemaphore renderFinishedSemaphore;
        VkFence inFlightFence;

        uint32_t imageIndex;
        const int MAX_FRAMES_IN_FLIGHT = 2;

        struct UniformBufferPreFrame {
            float time;
            float unused;
            float width;
            float height;
        };

        UniformBufferPreFrame ubo{};

        std::vector<VkBuffer> modelUniformBuffers;
        std::vector<VkDeviceMemory> modelUniformBuffersMemory;
        std::vector<void*> modelUniformBuffersMapped;

        std::vector<VkBuffer> deviceUniformBuffers;
        std::vector<VkDeviceMemory> deviceUniformBuffersMemory;
        std::vector<void*> deviceUniformBuffersMapped;

        std::chrono::time_point<std::chrono::steady_clock> startTime;
        float timeSinceStartup = 0;
	};
}

